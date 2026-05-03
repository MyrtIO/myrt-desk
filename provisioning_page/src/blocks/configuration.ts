export interface WifiConfiguration {
  ssid: string;
  password: string;
}

export interface MqttConfiguration {
  host: string;
  port: number;
  username: string;
  password: string;
}

export interface LightConfiguration {
  brightness_min: number;
  brightness_max: number;
  color_correction: number;
}

export interface Configuration {
  wifi: WifiConfiguration;
  mqtt: MqttConfiguration;
  light: LightConfiguration;
}

/** Recursively maps all primitive fields to HTMLInputElement or HTMLSelectElement */
type ToInputs<T> = {
  [K in keyof T]: T[K] extends object
    ? ToInputs<T[K]>
    : HTMLInputElement | HTMLSelectElement;
};

export interface ConfigurationBlockOptions {
  onDirty: () => void;
  onLightChange?: (light: LightConfiguration) => void;
}

export class ConfigurationBlock {
  private inputs: ToInputs<Configuration>;
  private isDirty: boolean = false;
  private form: HTMLFormElement;
  private colorPicker: HTMLInputElement;
  private onLightChange?: (light: LightConfiguration) => void;

  constructor(form: HTMLFormElement, options: ConfigurationBlockOptions) {
    this.onLightChange = options.onLightChange;

    const $ = (name: string): HTMLInputElement => {
      const input = form.querySelector<HTMLInputElement>(`[name=${name}]`);
      if (!input) {
        throw new Error(`Input with name ${name} not found`);
      }
      input.addEventListener("change", () => {
        if (!this.isDirty) {
          options.onDirty();
        }
        this.isDirty = true;
      });
      return input;
    };

    // Helper to register light field with auto-send
    const $light = (name: string): HTMLInputElement => {
      const input = $(name);
      input.addEventListener("change", () => this.emitLightChange());
      return input;
    };

    this.form = form;
    this.inputs = {
      wifi: {
        ssid: $("wifi_ssid"),
        password: $("wifi_password"),
      },
      mqtt: {
        host: $("mqtt_host"),
        port: $("mqtt_port"),
        username: $("mqtt_username"),
        password: $("mqtt_password"),
      },
      light: {
        brightness_min: $light("brightness_min"),
        brightness_max: $light("brightness_max"),
        color_correction: $light("color_correction"),
      },
    };

    // Setup color picker sync
    this.colorPicker = document.getElementById(
      "color_correction_picker",
    ) as HTMLInputElement;
    const hexInput = this.inputs.light.color_correction;

    // Use "change" event so auto-send only fires on commit, not while dragging
    this.colorPicker.addEventListener("change", () => {
      hexInput.value = this.colorPicker.value.toUpperCase();
      hexInput.dispatchEvent(new Event("change"));
    });

    hexInput.addEventListener("input", () => {
      let v = hexInput.value.toUpperCase().replace(/[^#0-9A-F]/g, "");
      if (!v.startsWith("#")) v = "#" + v.replace(/#/g, "");
      hexInput.value = v.slice(0, 7);
      if (/^#[0-9A-F]{6}$/.test(hexInput.value)) {
        this.colorPicker.value = hexInput.value;
      }
    });
  }

  /** Emit light configuration change if callback is set and color is valid */
  private emitLightChange() {
    if (!this.onLightChange) return;
    const hex = this.inputs.light.color_correction.value;
    // Skip if color is invalid (no alert for auto-send)
    if (!/^#[0-9A-F]{6}$/i.test(hex)) return;
    this.onLightChange(this.getLightValues());
  }

  /** Get current light configuration values */
  public getLightValues(): LightConfiguration {
    const hex = this.inputs.light.color_correction.value.replace("#", "");
    return {
      brightness_min: parseInt(this.inputs.light.brightness_min.value) || 0,
      brightness_max: parseInt(this.inputs.light.brightness_max.value) || 255,
      color_correction: parseInt(hex, 16) || 0xffffff,
    };
  }

  public unlock() {
    this.form.classList.remove("_disabled");
  }

  public lock() {
    this.form.classList.add("_disabled");
  }

  public markClean() {
    this.isDirty = false;
  }

  public setValues(configuration: Configuration) {
    // Format color_correction as #RRGGBB before recursive set
    const hex =
      "#" +
      configuration.light.color_correction
        .toString(16)
        .toUpperCase()
        .padStart(6, "0");
    this.colorPicker.value = hex;
    recursiveSetValues(this.inputs, {
      ...configuration,
      light: {
        ...configuration.light,
        color_correction: hex as unknown as number,
      },
    });
  }

  public validate(): boolean {
    const hex = this.inputs.light.color_correction.value;
    if (!/^#[0-9A-F]{6}$/i.test(hex)) {
      alert("Неверный формат цвета. Используйте #RRGGBB");
      return false;
    }
    return true;
  }

  public getValues(): Configuration {
    const values = recursiveGetValues(this.inputs);
    // Parse #RRGGBB back to u32
    const hex = (values.light.color_correction as unknown as string).replace(
      "#",
      "",
    );
    values.light.color_correction = parseInt(hex, 16) || 0xffffff;
    return values;
  }
}

function recursiveGetValues<T extends object>(inputs: ToInputs<T>): T {
  const values: T = {} as T;
  for (const [key, inputOrObject] of Object.entries(inputs)) {
    if (
      inputOrObject instanceof HTMLInputElement ||
      inputOrObject instanceof HTMLSelectElement
    ) {
      const input = inputOrObject as HTMLInputElement | HTMLSelectElement;
      if (input instanceof HTMLInputElement && input.type === "number") {
        values[key as keyof T] = parseInt(input.value) as T[keyof T];
      } else {
        values[key as keyof T] = input.value as T[keyof T];
      }
      continue;
    }
    values[key as keyof T] = recursiveGetValues(
      inputOrObject as ToInputs<T[keyof T]>,
    ) as T[keyof T];
  }
  return values;
}

function recursiveSetValues<T extends object>(inputs: ToInputs<T>, values: T) {
  for (const [key, valueOrObject] of Object.entries(values)) {
    if (typeof valueOrObject === "object") {
      recursiveSetValues(inputs[key as keyof T], valueOrObject as object);
      continue;
    }
    const input = inputs[key as keyof T] as
      | HTMLInputElement
      | HTMLSelectElement;
    input.value = String(valueOrObject);
  }
}
