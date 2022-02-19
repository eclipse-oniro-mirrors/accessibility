#ifndef ACCESSIBILE_ABILITY_MANAMGER_SERVICE_CAPTION_PROPERTY_PROXY_H
#define ACCESSIBILE_ABILITY_MANAMGER_SERVICE_CAPTION_PROPERTY_PROXY_H

#include <cstdint>

#include "accessible_ability_manager_service_caption_property_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceCaptionPropertyProxy : public IRemoteProxy<IAccessibleAbilityManagerServiceCaptionProperty> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibleAbilityManagerServiceCaptionPropertyProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceCaptionPropertyProxy() override;

    /**
     * @brief Receive the caption property notify from AAMS and send it to the observer registed..
     * @param caption the caption property
     * @return
     */
    virtual void OnCaptionPropertyChanged(const CaptionProperty &caption) override;
   private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<AccessibleAbilityManagerServiceCaptionPropertyProxy> delegator;
};

} //namespace Accessibility
} //namespace OHOS
#endif