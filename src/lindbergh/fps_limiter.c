#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "fps_limiter.h"

fps_limit fpsLimit;
double lastTime = 0.0;
int frameCount = 0;
double fps = 0.0;

double getTimeInMilliseconds()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
}

double getTimeInSeconds()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (double)time.tv_sec + (double)time.tv_usec / 1000000.0;
}

double calculateFps()
{
    double currentTime = getTimeInSeconds();
    double deltaTime = currentTime - lastTime;
    frameCount++;
    if (deltaTime >= 1.0)
    {
        fps = frameCount / deltaTime;
        frameCount = 0;
        lastTime = currentTime;
    }
    return fps;
}

long Clock_now()
{
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return time_now.tv_sec * 1000000L + time_now.tv_usec;
}

void FpsLimiter(fps_limit *stats)
{
    stats->sleepTime = stats->targetFrameTime - (stats->frameStart - stats->frameEnd);

    if (stats->sleepTime > stats->frameOverhead)
    {
        long adjustedSleep = stats->sleepTime - stats->frameOverhead;

        usleep(adjustedSleep);

        stats->frameOverhead = (Clock_now() - stats->frameStart) - adjustedSleep;

        if (stats->frameOverhead > stats->targetFrameTime / 2)
        {
            stats->frameOverhead = 0;
        }
    }
}