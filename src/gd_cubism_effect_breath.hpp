#ifndef GD_CUBISM_EFFECT_BREATH
#define GD_CUBISM_EFFECT_BREATH


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <CubismFramework.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Id/CubismIdManager.hpp>

#include <gd_cubism_effect.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
class GDCubismEffectBreath : public GDCubismEffect {
    GDCLASS(GDCubismEffectBreath, GDCubismEffect);

protected:
    static void _bind_methods() {}

private:
    Csm::CubismBreath* _breath = nullptr;

public:
    virtual void _cubism_init(Csm::ICubismModelSetting* _model_setting) override {
        this->_breath = Csm::CubismBreath::Create();

        Csm::csmVector<Csm::CubismBreath::BreathParameterData> param;

        param.PushBack(Csm::CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamAngleX), 0.0f, 15.0f, 6.5345f, 0.5f));
        param.PushBack(Csm::CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamAngleY), 0.0f, 8.0f, 3.5345f, 0.5f));
        param.PushBack(Csm::CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamAngleZ), 0.0f, 10.0f, 5.5345f, 0.5f));
        param.PushBack(Csm::CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBodyAngleX), 0.0f, 4.0f, 15.5345f, 0.5f));
        param.PushBack(Csm::CubismBreath::BreathParameterData(CubismFramework::GetIdManager()->GetId(ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

        this->_breath->SetParameters(param);
    }

    virtual void _cubism_term() override {
        if(this->_breath != nullptr) {
            Csm::CubismBreath::Delete(this->_breath);
            this->_breath = nullptr;
        }
    }

    virtual void _cubism_process(Csm::CubismModel* model, const float delta) override {
        if(this->_breath == nullptr) return;
        if(this->_active == false) return;
        this->_breath->UpdateParameters(model, delta);
    }
};


#endif // GD_CUBISM_EFFECT_BREATH
