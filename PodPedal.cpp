#include "daisy_pod.h"
#include "daisysp.h"
#include "Timer.h"
#include "Display.h"
#include "ScopeRingBuffer.h"

using namespace daisy;
using namespace daisysp;

static DaisyPod hw;

Parameter param_amp_scaling, param_wavelength_scaling;
static constexpr float PARAM_MIN_AMP_SCALING = 1.0f;
static constexpr float PARAM_MAX_AMP_SCALING = 10.0f;
static constexpr float PARAM_MIN_WAVELENGTH_SCALING = 0.1f;
static constexpr float PARAM_MAX_WAVELENGTH_SCALING = 10.0f;

static Display oled;

static constexpr int DISPLAY_REFRESH_RATE_FPS = 30;
static constexpr int DISPLAY_REFRESH_TIMEOUT_MS = 1000 / DISPLAY_REFRESH_RATE_FPS;
Timer display_timer(DISPLAY_REFRESH_TIMEOUT_MS);

static constexpr auto AUDIO_SAMPLE_RATE = SaiHandle::Config::SampleRate::SAI_48KHZ;

// TODO: move all Oscilloscope functionality to a class
// the sample buffer is oversampled to get a smoother waveform
static constexpr int OVERSAMPLE_FACTOR = 16;
const bool DRAW_CENTER_LINE = false;

static const int BUFFER_SIZE = oled.Width() * OVERSAMPLE_FACTOR;
ScopeRingBuffer scope_buffer(BUFFER_SIZE);

float limit = 0.3f;
static constexpr float LIMIT_STEP = 0.0025f;
static constexpr float MAX_LIMIT = 1.0f;
static constexpr float MIN_LIMIT = 0.0f;
bool is_limit_enabled = false;

void InitParams()
{
    param_amp_scaling.Init(hw.knob1,
                           PARAM_MIN_AMP_SCALING,
                           PARAM_MAX_AMP_SCALING,
                           Parameter::LINEAR);
    param_wavelength_scaling.Init(hw.knob2,
                                  PARAM_MIN_WAVELENGTH_SCALING,
                                  PARAM_MAX_WAVELENGTH_SCALING,
                                  Parameter::LINEAR);
}

void ProcessParams()
{
    param_amp_scaling.Process();
    param_wavelength_scaling.Process();
}

void HandleControls()
{
    hw.ProcessAllControls();
    limit += hw.encoder.Increment() * LIMIT_STEP;
    if (limit > MAX_LIMIT)
        limit = MAX_LIMIT;
    if (limit < MIN_LIMIT)
        limit = MIN_LIMIT;

    if (hw.encoder.RisingEdge())
        is_limit_enabled = !is_limit_enabled;
}

void DrawScope()
{
    // Copy the scope buffer into a display buffer to prevent timing issues
    int w = oled.Width();
    float display_buffer[w];
    float step_scaling = param_wavelength_scaling.Value();
    int step = scope_buffer.Size() / w / step_scaling;
    for (int i = 0; i < w; i++)
    {
        int buffer_index = i * step;
        display_buffer[i] = scope_buffer.GetFromHead(buffer_index);
    }

    // Draw the waveform
    int wave_y_min = oled.TOP_ZONE_HEIGHT + 1;
    int wave_y_max = oled.Height() - 1;
    int wave_y_center = (wave_y_min + wave_y_max) / 2;
    // crest to center
    int wave_height = (wave_y_max - wave_y_min);

    // draw the center line in the blue zone
    if (DRAW_CENTER_LINE == true)
    {
        for (int x = 0; x < w; x++)
        {
            if (x % 2 == 0)
                oled.display.DrawPixel(x, wave_y_center, true);
        }
    }

    // draw the waveform
    for (int x = 0; x < w; x++)
    {
        float sig = display_buffer[x];
        // offset (from -1..1) from y = 0
        float y_offset = (sig) * -1;

        // scale and map to actual pixel offset
        float amplitude_scaling = param_amp_scaling.Value();
        float y_offset_scaled = y_offset * wave_height / 2 * amplitude_scaling;
        int y = wave_y_center + y_offset_scaled;

        // skip if out of bounds
        if (y < wave_y_min || y > wave_y_max || x < 0 || x >= w)
            continue;

        // add pixel to display
        oled.display.DrawPixel(x, y, true);
    }
}

// TODO: move to a DisplayHeader class
void DisplayHeaders()
{
    // NOTE: my display has a 16px high yellow zone at the top for text.
    //  I can use two lines of Font_6x8 starting at (0,0) and (0,9) without going out of bounds of this zone

    int buffer_size = 32;
    char buf[buffer_size];
    // NOTE: I was using %f to print the float but nothing was showing
    // ChatGPT wrote:
    // >  In many embedded toolchains (ARM GCC, newlib-nano, etc.),
    // >  %f formatting is disabled by default to save flash space.
    // >  When that happens break the float into parts and use `"Avg Sig: %d.%03d"`
    int whole = (int)limit;
    int frac = (int)((limit - whole) * 10000);
    snprintf(buf,
             buffer_size,
             "limit,: %d.%04d | %s",
             whole,
             frac,
             is_limit_enabled ? "ON" : "OFF");
    oled.display.SetCursor(0, 0);
    oled.display.WriteString(buf, Font_6x8, true);
}

void UpdateDisplay()
{
    // Clear the display
    ProcessParams();

    oled.display.Fill(false);

    DrawScope();
    DisplayHeaders();

    // render the display
    oled.display.Update();
}

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
    HandleControls();
    for (size_t i = 0; i < size; i++)
    {
        // NOTE: dropping right channel which seems to be silent on my guitars (either mono input or a cable issue)
        float signal_l = in[0][i];
        float signal_l_processed = signal_l;

        // apply limiting if enabled
        if (is_limit_enabled)
        {
            if (signal_l_processed > limit)
                signal_l_processed = limit;
            else if (signal_l_processed < -limit)
                signal_l_processed = -limit;
        }

        out[0][i] = signal_l_processed;
        out[1][i] = signal_l_processed;

        scope_buffer.Push(signal_l_processed);
    }
}

int main(void)
{
    hw.Init();

    // Initialize parameters
    InitParams();

    // Init the display
    oled.Init();

    // Startup audio loop
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(AUDIO_SAMPLE_RATE);
    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    // Audio processing i/o is handled by the AudioCallback so this main loop is for other ops
    while (1)
    {
        // LEARNING: Using a timer is better than simply doing UpdateDisplay(); System::Delay(DISPLAY_REFRESH_TIMEOUT_MS);
        //           because it avoids blocking the main thread
        if (display_timer.Ready())
        {
            UpdateDisplay();
        }
    }
}
