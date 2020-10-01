#pragma once
#pragma comment(lib, "pluginsdk.lib")
#include "bakkesmod/plugin/bakkesmodplugin.h"

class GroundGameTrainer : public BakkesMod::Plugin::BakkesModPlugin
{
private:
    std::shared_ptr<bool> bEnabled;
    std::shared_ptr<bool> bRender;
    std::shared_ptr<float> cvarHeightThreshold;
    std::shared_ptr<float> cvarDownwardForce;

    bool bIsCarAboveThreshold = false;

public:
	void onLoad() override;
	void onUnload() override;
    
    void OnEnabledChanged();
    void Render(CanvasWrapper canvas);
    void Tick();
};
