#pragma once

#include <vector>

typedef struct model_s model_t;

float StudioGetAnimationDuration(model_t* model, int anim);
float StudioGetAnimationDuration(int modelIndex, int anim);

void StudioGetAnimationDurations(model_t* model, std::vector<float>& outDurations);
void StudioGetAnimationDurations(int modelIndex, std::vector<float>& outDurations);
