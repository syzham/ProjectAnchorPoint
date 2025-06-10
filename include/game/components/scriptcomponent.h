#ifndef PROJECTANCHORPOINT_SCRIPTCOMPONENT_H
#define PROJECTANCHORPOINT_SCRIPTCOMPONENT_H

#include "game/components/component.h"
#include "game/ScriptBehaviour.h"


class ScriptComponent : public Component {
public:

    std::string getName() override;

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;

};

REGISTER_COMPONENT(ScriptComponent);

#endif //PROJECTANCHORPOINT_SCRIPTCOMPONENT_H
