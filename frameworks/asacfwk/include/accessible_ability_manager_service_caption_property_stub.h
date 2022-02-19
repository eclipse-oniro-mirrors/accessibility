#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_STUB_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_STUB_H

#include <cstdint>
#include <list>

#include "accessible_ability_manager_service_caption_property_interface.h"
#include "accessibility_errorcode.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceCaptionPropertyStub : public IRemoteStub<IAccessibleAbilityManagerServiceCaptionProperty> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    AccessibleAbilityManagerServiceCaptionPropertyStub();

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceCaptionPropertyStub() = default;

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @param option The option paramter of IPC,such as: async,sync
     * @return
     */
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

    /**
     * @brief Receive the caption property notify from AAMS and send it to the observer registed..
     * @param caption The caption property.
     * @return
     */
    void OnCaptionPropertyChanged(const CaptionProperty &caption) override;
private:
    /**
     * @brief Handle the IPC request for the function:HandleOnStateChanged.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleOnCaptionPropertyChanged(MessageParcel &data, MessageParcel &reply);

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityManagerServiceCaptionPropertyStub);
};

} //namespace Accessibility
} //namespace OHOS
#endif
