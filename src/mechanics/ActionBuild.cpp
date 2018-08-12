#include "ActionBuild.h"

#include <genie/dat/Unit.h>

ActionBuild::ActionBuild(const Unit::Ptr &builder, const Unit::Ptr &building) : IAction(Type::Build, builder),
    m_targetBuilding(building)
{
}

ActionBuild::~ActionBuild()
{
    Unit::Ptr building = m_targetBuilding.lock();

    if (building && m_prevTime) {
        building->constructors--;
    }
}

bool ActionBuild::update(Time time)
{
    Unit::Ptr unit = m_unit.lock();
    if (!unit) {
        WARN << "Unit gone";
        return true;
    }

    Unit::Ptr building = m_targetBuilding.lock();
    if (!building) {
        unit->removeAction(this);
        return true;
    }

    if (!m_prevTime) {
        m_prevTime = time;
        building->constructors++;
        return false;
    }

    if (building->creationProgress() >= 1.) {
        DBG << "building already finished";
        unit->removeAction(this);
        return true;
    }

    float progress = 3. / (building->constructors + 2.);
    progress *= (time - m_prevTime) * 0.0015;
    m_prevTime = time;

    building->increaseCreationProgress(progress);

    if (building->creationProgress() >= 1.) {
        DBG << "building finished";
        unit->removeAction(this);
        return true;
    }

    return false;
}

IAction::UnitState ActionBuild::unitState() const
{
    std::shared_ptr<Unit> target = m_targetBuilding.lock();
    if (!target) {
        WARN << "target lost";
        return Idle;
    }

    if (target->data()->ID == Unit::Farm) {
        return Working;
    }

    return Proceeding;
}
