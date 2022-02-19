#ifndef ACCESSIBLE_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_INTERFACE_H
#define ACCESSIBLE_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_INTERFACE_H

#include <cstdint>

#include "iremote_broker.h"
#include "iremote_object.h"
#include "accessibility_caption.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityManagerServiceCaptionProperty: public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerServiceCaptionProperty");

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registed..
     * @param caption The caption properties.
     * @return
     */
    virtual void OnCaptionPropertyChanged(const CaptionProperty& caption) = 0;

    enum class Message {
        ON_CAPTION_PROPERTY_CHANGED,
    };
};

} //namespace Accessibility
} //namespace OHOS
#endif