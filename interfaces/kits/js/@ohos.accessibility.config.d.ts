import accessibility from './@ohos.accessibility';
import { AsyncCallback, Callback } from './basic';
 
/**
 * Configuration of the accessibility.
 *
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 */
declare namespace config {
  /**
   * Indicates the configuration of high-contrast text.
   */
  var highContrastText: Config<boolean>;
  /**
   * Indicates the configuration of invert color.
   */
  var invertColor: Config<boolean>;
  /**
   * Indicates the configuration of daltonization color filter.
   */
  var daltonizationColorFilter: Config<DaltonizationColorFilter>;
  /**
   * Indicates the configuration of content timeout.
   */
  var contentTimeout: Config<number>;
  /**
   * Indicates the configuration of animation state.
   */
  var animationOff: Config<boolean>;
  /**
   * Indicates the configuration of brightness discount.
   */
  var brightnessDiscount: Config<number>;
  /**
   * Indicates the configuration of screen magnifier.
   */
  var screenMagnifier: Config<boolean>;
  /**
   * Indicates the configuration of audio mono.
   */
  var audioMono: Config<boolean>;
  /**
   * Indicates the configuration of audio balance.
   */
  var audioBalance: Config<number>;
  /**
   * Indicates the configuration of monse key state.
   */
  var mouseKey: Config<boolean>;
  /**
   * Indicates the configuration of mouse auto click.
   */
  var mouseAutoClick: Config<number>;
  /**
   * Indicates the configuration of shortkey state.
   */
  var shortkey: Config<boolean>;
  /**
   * Indicates the configuration of shortkey target.
   */
  var shortkeyTarget: Config<string>;
  /**
   * Indicates the configuration of captions state.
   */
  var captions: Config<boolean>;
  /**
   * Indicates the configuration of captions style.
   */
  var captionsStyle: Config<accessibility.CaptionsStyle>;
 
  /**
   * Enable the acceessibility extension ability.
   * @param name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   * @param capability Indicates the ability.
   */
  function enableAbility(name: string, capability: Array<accessibility.Capability>): Promise<void>;
  function enableAbility(name: string, capability: Array<accessibility.Capability>, callback: AsyncCallback<void>): void;
 
  /**
   * Disable the acceessibility extension ability.
   * @param name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   */
  function disableAbility(name: string): Promise<void>;
  function disableAbility(name: string, callback: AsyncCallback<void>): void;
 
  /**
   * Register the listener to listen for for changes in the enabled status of accessibility extensions.
   * @param type Indicates the enableAbilityListsStateChanged type.
   * @param callback Indicates the listener.
   */
  function on(type: 'enableAbilityListsStateChanged', callback: Callback<void>): void;
 
  /**
   * Deregister listener that watch for changes in the enabled status of accessibility extensions.
   * @param type Indicates the enableAbilityListsStateChanged type.
   * @param callback Indicates the listener.
   */
  function off(type: 'enableAbilityListsStateChanged', callback?: Callback<void>): void;
 
  /**
   * Indicates setting, getting, and listening to changes in configuration.
   */
  interface Config<T> {
    /**
     * Setting configuration value.
     * @param value Indicates the value.
     */
    set(value: T): Promise<void>;
    set(value: T, callback: AsyncCallback<void>): void;
 
    /**
     * Getting configuration value.
     */
    get(): Promise<T>;
    get(callback: AsyncCallback<T>): void;
 
    /**
     * Register the listener to listen for configuration changes.
     * @param callback Indicates the listener.
     */
    on(callback: Callback<T>): void;
 
    /**
     * Deregister the listener to listen for configuration changes.
     * @param callback Indicates the listener.
     */
    off(callback?: Callback<T>): void;
  }
 
  /**
   * Indicates the type of daltonization color filter.
   */
  type DaltonizationColorFilter = 'Normal' | 'Protanomaly' | 'Deuteranomaly' | 'Tritanomaly';
}
export default config;