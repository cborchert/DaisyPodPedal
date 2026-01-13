#include <vector>

class ScopeRingBuffer
{
    std::vector<float> buffer_;
    size_t head_ = 0;

public:
    ScopeRingBuffer(size_t size) : buffer_(size, 0.0f /* fill with zeroes*/) {}

    void Push(float val)
    {
        buffer_[head_] = val;
        head_ = (head_ + 1) % buffer_.size(); // wrap around to the start of the buffer based on buffer size
    }

    // LEARNING: the const here means that we will not modify the member values
    float Get(size_t index) const { return buffer_[index % buffer_.size()]; }
    float GetFromHead(size_t index) const
    {
        size_t i = (head_ + buffer_.size() - 1 - index) % buffer_.size();
        return buffer_[i];
    }
    size_t Size() const { return buffer_.size(); }
    void Fill(float val) { std::fill(buffer_.begin(), buffer_.end(), val); }
};