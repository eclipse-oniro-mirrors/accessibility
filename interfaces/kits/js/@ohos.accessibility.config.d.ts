import accessibility from './@ohos.accessibility';
import { AsyncCallback, Callback } from './basic';

declare namespace config {
  var highContrastText: Config<boolean>;
  var invertColor: Config<boolean>;
  var daltonizationColorFilter: Config<DaltonizationColorFilter>;
  // number int scope: 0 - 5000  ms
  var contentTimeout: Config<number>;
  var animationOff: Config<boolean>;
  // number float scope: 0.0 - 1.0
  var brightnessDiscount: Config<number>;
  var screenMagnifier: Config<boolean>;
  var audioMono: Config<boolean>;
  // number float scope: -1.0 - 1.0 
  var audioBalance: Config<number>;
  var mouseKey: Config<boolean>;
  // number int scope: -1 off / 0 - 5000 ms on   
  var mouseAutoClick: Config<number>;
  var shortkey: Config<boolean>;
  var shortkey_target: Config<string>;
  var caption: Config<boolean>;
  var captionStyle: Config<accessibility.CaptionsStyle>;

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
   * Register the observe of the AbilityLists enable status changed.
   * @since 9
   * @sysCap Accessibility
   * @param -
   * @return -
   */  
  function on(type: 'enableAbilityListsStateChanged', callback: Callback<void>): void;
  
   /**
   * Deregister the observe of the AbilityLists enable status changed.
   * @since 9
   * @sysCap Accessibility
   * @param -
   * @return -
   */  
  function off(type: 'enableAbilityListsStateChanged', callback?: Callback<void>): void;

  interface Config<T> {
    // permission: accessibility.config.write
    set(value: T, callback: AsyncCallback<void>): void;
    set(value: T): Promise<void>;
    // permission: accessibility.config.read
    get(callback: AsyncCallback<T>): void;
    get(): Promise<T>;
    // permission: accessibility.config.read
    on(callback: Callback<T>): void;
    off(callback?: Callback<T>): void;
  }

  type DaltonizationColorFilter = 'Normal' | 'Protanomaly' | 'Deuteranomaly' | 'Tritanomaly';
}
export default config;