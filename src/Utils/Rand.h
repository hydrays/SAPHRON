#pragma once


static std::random_device rd;
static std::mt19937 gen{rd()};
static std::uniform_real_distribution<> runif{0.0, 1.0};
static std::normal_distribution<> rnorm{0.0, 1.0};
static std::uniform_int_distribution<> rid{1, 32767};

class Rand
{
public:
    
    Rand(unsigned int i = 0)
    {
	gen.seed(i);
    }

    double doub()
    {
	return runif(gen);
    }

    double gauss()
    {
	return rnorm(gen);
    }

    unsigned int int32()
    {
	return (unsigned int) rid(gen);
    }
    
    void seed(int i)
    {
	return gen.seed(i);
    }
};
