# Accessibility

# Introduction<a name="intro"></a>

The accessibility framework provides a standard mechanism for exchanging information between applications and accessibility applications and supports the development of accessibility applications to enhance the accessibility experience. The typical use cases are as follows:

1. Allow people with disabilities to use applications. For example, provide the screen reader for people with visual impairments.
1. Provide the capability to interact with applications. For example, offer the UI automatic test framework and the capability to develop accessibility applications based on UI responses.

![](./figures/AccessibilityFramework.png)

# Directory Structure<a name="files"></a>

```
/foundation/barrierfree/accessibility
├── frameworks
│   ├── aafwk               # AccessibleAbility Kit implementation code
│   ├── acfwk               # AccessibilityConfiguration Kit implementation code
│   └── asacfwk             # AccessibleAbility Kit implementation code
├── interfaces
│   ├── innerkits           # Internal APIs of AccessibleAbility、AccessibilityConfiguration and AccessibleAbilityClient
│   └── kits                # Developer APIs of AccessibleAbility、AccessibilityConfiguration and AccessibleAbilityClient
├── sa_profile              # System service configuration files
└── services
    └── aams                # Framework code of the AccessibleAbility management service
    └── interface           # Interface of the AccessibleAbility management service
    └── test                # Test code
```

# Repositories Involved<a name="relevant"></a>

**accessibility**

[ability_ability_runtime](https://gitee.com/openharmony/ability_ability_runtime)

[account_os_account](https://gitee.com/openharmony/account_os_account)

[bundlemanager_bundle_framework](https://gitee.com/openharmony/bundlemanager_bundle_framework)

[distributeddatamgr_appdatamgr](https://gitee.com/openharmony/distributeddatamgr_appdatamgr)

[notification_common_event_service](https://gitee.com/openharmony/notification_common_event_service)

[multimodalinput_input](https://gitee.com/openharmony/multimodalinput_input)

[windowmanager](https://gitee.com/openharmony/windowmanager)
