#include "GroundGameTrainer.h"
#include "bakkesmod\wrappers\includes.h"
#include <sstream>
#include <iomanip>

BAKKESMOD_PLUGIN(GroundGameTrainer, "Limit freeplay practice to the ground", "1.0", PLUGINTYPE_FREEPLAY)

void GroundGameTrainer::onLoad()
{
	using namespace std::placeholders;
	
    bEnabled = std::make_shared<bool>(false);
    CVarWrapper EnabledCvar = cvarManager->registerCvar("GroundGameTrainer_Enabled", "0", "Enable the GroundGameTrainer plugin", true, true, 0, true, 1);
    EnabledCvar.bindTo(bEnabled);
    EnabledCvar.addOnValueChanged(std::bind(&GroundGameTrainer::OnEnabledChanged, this));

    bRender = std::make_shared<bool>(false);
    cvarHeightThreshold = std::make_shared<float>(0.f);
    cvarDownwardForce = std::make_shared<float>(0.f);
    cvarManager->registerCvar("GroundGameTrainer_Render", "1", "Render text showing values", true, true, 0, true, 1).bindTo(bRender);
    cvarManager->registerCvar("GroundGameTrainer_HeightThreshold", "350", "Threshold above which car will be pushed downward", true, true, 0, true, 2500).bindTo(cvarHeightThreshold);
    cvarManager->registerCvar("GroundGameTrainer_DownwardForce", "3500", "Force with which car will be pushed downward", true, true, 0, true, 5000).bindTo(cvarDownwardForce);
	
    gameWrapper->RegisterDrawable(std::bind(&GroundGameTrainer::Render, this, _1));
}
void GroundGameTrainer::onUnload() {}

void GroundGameTrainer::OnEnabledChanged()
{
    if(*bEnabled) { return; }
    
    CarWrapper car = gameWrapper->GetLocalCar();
    if(!car.IsNull())
    {
        //Reset sticky to defaults
        car.SetStickyForce({.5f, 1.5f});
    }
}

void GroundGameTrainer::Render(CanvasWrapper canvas)
{
    if(!(*bEnabled) || !gameWrapper->IsInFreeplay()) { return; }

    Tick();

    if(!(*bRender)) { return; }

    canvas.SetColor(LinearColor{0,255,0,255});

    static const std::string GroundGameText = "GroundGameTrainer is enabled";
    std::stringstream ValuesText;
    ValuesText << std::fixed << std::setprecision(2) << "Height Threshold: " << *cvarHeightThreshold << ", Downward Force: " << *cvarDownwardForce;
    static const std::string IsAboveThresholdText = "Car is above threshold";

    Vector2F GroundGameTextSizeF = canvas.GetStringSize(GroundGameText, 2);
    Vector2F ValuesTextSizeF = canvas.GetStringSize(ValuesText.str(), 2);
    Vector2F IsAboveThresholdSizeF = canvas.GetStringSize(IsAboveThresholdText, 2);

    Vector2 GroundGameTextSize = {static_cast<int>(GroundGameTextSizeF.X), static_cast<int>(GroundGameTextSizeF.Y)};
    Vector2 ValuesTextSize = {static_cast<int>(ValuesTextSizeF.X), static_cast<int>(ValuesTextSizeF.Y)};
    Vector2 IsAboveThresholdSize = {static_cast<int>(IsAboveThresholdSizeF.X), static_cast<int>(IsAboveThresholdSizeF.Y)};

    Vector2 Middle = {canvas.GetSize().X / 2, 50};

    Vector2 GroundGameTextPosition = Middle - (GroundGameTextSize / 2);
    canvas.SetPosition(GroundGameTextPosition);
    canvas.DrawString(GroundGameText, 2, 2);

    Vector2 ValuesTextPosition = Middle - (ValuesTextSize / 2) + Vector2{0, GroundGameTextSize.Y + 10};
    canvas.SetPosition(ValuesTextPosition);
    canvas.DrawString(ValuesText.str(), 2, 2);

    if(bIsCarAboveThreshold)
    {
        canvas.SetColor(LinearColor{255,0,0,255});
        Vector2 IsAboveThresholdTextPosition = Middle - (IsAboveThresholdSize / 2) + Vector2{0, GroundGameTextSize.Y + ValuesTextSize.Y + 20};
        canvas.SetPosition(IsAboveThresholdTextPosition);
        canvas.DrawString(IsAboveThresholdText, 2, 2);
    }
}

void GroundGameTrainer::Tick()
{
    bIsCarAboveThreshold = false;

    CarWrapper car = gameWrapper->GetLocalCar();
    if(car.IsNull()) { return; }

    //Remove sticky force from walls
    if(*bEnabled)
    {
        car.SetStickyForce({.5f, -2.f});
    }

    if(car.GetLocation().Z <= *cvarHeightThreshold) { return; }
    
    //Push car down if it is above height threshold
    bIsCarAboveThreshold = true;
    Vector ForceDirection = Vector(0,0,-1) * *cvarDownwardForce;
    car.AddForce(ForceDirection, 1);
}
