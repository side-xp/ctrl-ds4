/**
 * Controller tester of the documentation website, which reads the controllers with the Gamepad API.
 *
 * The page defines the structure of the tester (see tester.md), and this script only fills it in,
 * through these data attributes:
 * - data-gamepad-tester: root of the tester.
 * - data-gt-when="<conditions>": element shown only while all its space-separated conditions are
 *   met (see getConditions()). data-gt-open-when does the same to open a <details> element.
 * - data-gt-button="<index>": element showing a button, given the data-pressed attribute while
 *   the button is pressed, and its value (from 0 to 1) as the --gt-value CSS property.
 * - data-gt-axis="<index>": element showing an axis, given its value (from -1 to 1) as --gt-value.
 * - data-gt-stick="<X index> <Y index>": element showing a stick, given the values of its axes as
 *   --gt-x and --gt-y.
 * - data-gt-button-value="<index>", data-gt-axis-value="<index>": element showing the value as text.
 * - data-gt-field="<name>": element showing a property of the controller as text (see FIELDS).
 * - data-gt-select: <select> picking the controller to show when several are connected.
 * - data-gt-raw="buttons|axes": list filled with one copy of the <template> with the same
 *   data-gt-template value per button or axis. In the copies, the index is set on the empty
 *   data-gt-button, data-gt-axis, data-gt-button-value and data-gt-axis-value attributes, and
 *   replaces {index} in the text of data-gt-label.
 * - data-gt-rumble-input="<parameter>": range input of a parameter of the rumble (see
 *   readRumbleParameters()), its value shown in the data-gt-rumble-output element of the same
 *   parameter. data-gt-rumble-play and data-gt-rumble-stop are the buttons playing and stopping it.
 *
 * The website uses instant navigation, which replaces the page content without reloading the
 * scripts: the testers are set up each time document$ (from the theme) emits a new page.
 */

(() => {
  // #region Constants

  const ROOT_SELECTOR = '[data-gamepad-tester]';
  // Effect of Gamepad.vibrationActuator that drives the two motors of a DS4.
  const RUMBLE_EFFECT = 'dual-rumble';
  // Decimals of the values shown as text: enough to see a stick drift, without flickering.
  const VALUE_DECIMALS = 2;
  // Placeholder of the index in the text of the data-gt-label elements of the templates.
  const INDEX_PLACEHOLDER = '{index}';
  // Duration of the rumble if the page has no input for it: long enough to be felt.
  const DEFAULT_RUMBLE_DURATION_MS = 1000;

  // Properties of the controller shown by the data-gt-field elements, by field name.
  const FIELDS = {
    id: (gamepad) => gamepad.id,
    index: (gamepad) => gamepad.index,
    mapping: (gamepad) => gamepad.mapping || 'none',
    buttons: (gamepad) => gamepad.buttons.length,
    axes: (gamepad) => gamepad.axes.length,
  };

  // #endregion

  // #region Setup

  // Roots of the testers already set up, so a page emitted twice doesn't start two loops.
  const startedRoots = new WeakSet();

  /** Sets up the testers of the current page that aren't already. */
  function setupTesters() {
    for (const root of document.querySelectorAll(ROOT_SELECTOR)) {
      if (!startedRoots.has(root)) {
        startedRoots.add(root);
        startTester(root);
      }
    }
  }

  /**
   * Starts a tester: wires its inputs, then updates it on every frame until it leaves the page.
   *
   * @param {HTMLElement} root Root of the tester.
   */
  function startTester(root) {
    const tester = {
      root,
      // Index of the controller shown, or null if none is.
      shownIndex: null,
      // Key of the connected controllers the picker lists (see getGamepadsKey()).
      pickerKey: null,
      // Value last shown by each element, so unchanged values don't touch the page.
      shownValues: new WeakMap(),
      hooks: null,
    };
    if (!isGamepadApiAvailable()) {
      applyConditions(root, new Set(['unsupported']));
      return;
    }

    tester.hooks = collectHooks(root);
    wireRumble(tester);
    const select = root.querySelector('[data-gt-select]');
    select?.addEventListener('change', () => {
      tester.shownIndex = Number(select.value);
      tester.pickerKey = null;
    });

    const update = () => {
      // The page was left (instant navigation removes its content): stop the loop.
      if (!root.isConnected) {
        return;
      }
      updateTester(tester);
      requestAnimationFrame(update);
    };
    update();
  }

  /**
   * Tells whether the browser gives the controllers to this page.
   *
   * @returns {boolean} False if the browser has no Gamepad API, or hides it from this page (it does
   *   when the page isn't served over HTTPS, or when a permissions policy blocks it).
   */
  function isGamepadApiAvailable() {
    if (!window.isSecureContext || typeof navigator.getGamepads !== 'function') {
      return false;
    }
    try {
      navigator.getGamepads();
      return true;
    } catch {
      return false;
    }
  }

  // #endregion

  // #region Controllers

  /**
   * Gets the connected controllers.
   *
   * @returns {Gamepad[]} The connected controllers, by increasing index.
   */
  function getConnectedGamepads() {
    return Array.from(navigator.getGamepads()).filter((gamepad) => gamepad?.connected);
  }

  /**
   * Builds a key that changes whenever a controller is connected or disconnected.
   *
   * @param {Gamepad[]} gamepads Connected controllers.
   * @returns {string} The key.
   */
  function getGamepadsKey(gamepads) {
    return gamepads.map((gamepad) => `${gamepad.index}:${gamepad.id}`).join('\n');
  }

  /**
   * Tells whether a controller can rumble from this page.
   *
   * @param {Gamepad} gamepad Controller to check.
   * @returns {boolean} True if it can play the RUMBLE_EFFECT.
   */
  function canRumble(gamepad) {
    const actuator = gamepad.vibrationActuator;
    if (!actuator) {
      return false;
    }
    // Older browsers only give the type of the actuator, not the list of its effects.
    return Array.isArray(actuator.effects)
      ? actuator.effects.includes(RUMBLE_EFFECT)
      : actuator.type === RUMBLE_EFFECT;
  }

  // #endregion

  // #region Update

  /**
   * Updates a tester from the current state of the controllers.
   *
   * @param {object} tester Tester to update (see startTester()).
   */
  function updateTester(tester) {
    const gamepads = getConnectedGamepads();
    const gamepad = gamepads.find((connected) => connected.index === tester.shownIndex) ?? gamepads[0] ?? null;

    const key = getGamepadsKey(gamepads);
    if (key !== tester.pickerKey || (gamepad?.index ?? null) !== tester.shownIndex) {
      tester.pickerKey = key;
      showGamepad(tester, gamepad, gamepads);
    }
    if (gamepad) {
      updateValues(tester, gamepad);
    }
  }

  /**
   * Shows another controller, or the connected controllers changed: updates the parts of the
   * tester that don't change while a controller is shown.
   *
   * @param {object} tester Tester to update (see startTester()).
   * @param {Gamepad | null} gamepad Controller to show, or null if none is connected.
   * @param {Gamepad[]} gamepads Connected controllers.
   */
  function showGamepad(tester, gamepad, gamepads) {
    const { root } = tester;
    tester.shownIndex = gamepad?.index ?? null;
    tester.shownValues = new WeakMap();
    applyConditions(root, getConditions(gamepad, gamepads));
    if (!gamepad) {
      return;
    }

    fillPicker(root.querySelector('[data-gt-select]'), gamepad, gamepads);
    for (const element of root.querySelectorAll('[data-gt-field]')) {
      const field = FIELDS[element.dataset.gtField];
      element.textContent = field ? field(gamepad) : '';
    }
    fillRawList(root, 'buttons', gamepad.buttons.length);
    fillRawList(root, 'axes', gamepad.axes.length);
    tester.hooks = collectHooks(root);
  }

  /**
   * Gets the conditions met by the tester, for its data-gt-when and data-gt-open-when elements.
   *
   * @param {Gamepad | null} gamepad Controller shown, or null if none is connected.
   * @param {Gamepad[]} gamepads Connected controllers.
   * @returns {Set<string>} 'waiting' if no controller is connected. Otherwise 'connected', with
   *   'standard' or 'nonstandard' for the mapping of the controller shown, 'rumble' or 'no-rumble'
   *   for its rumble, and 'several' if several controllers are connected.
   */
  function getConditions(gamepad, gamepads) {
    if (!gamepad) {
      return new Set(['waiting']);
    }
    return new Set([
      'connected',
      gamepad.mapping === 'standard' ? 'standard' : 'nonstandard',
      canRumble(gamepad) ? 'rumble' : 'no-rumble',
      gamepads.length > 1 ? 'several' : 'single',
    ]);
  }

  /**
   * Shows the data-gt-when elements whose conditions are met and hides the others, and opens the
   * data-gt-open-when elements whose conditions are met (they're never closed, to let the user
   * close them).
   *
   * @param {HTMLElement} root Root of the tester.
   * @param {Set<string>} conditions Conditions met (see getConditions()).
   */
  function applyConditions(root, conditions) {
    const areMet = (value) => value.split(/\s+/).every((condition) => conditions.has(condition));
    for (const element of root.querySelectorAll('[data-gt-when]')) {
      element.hidden = !areMet(element.dataset.gtWhen);
    }
    for (const element of root.querySelectorAll('[data-gt-open-when]')) {
      if (areMet(element.dataset.gtOpenWhen)) {
        element.open = true;
      }
    }
    root.dataset.gtReady = '';
  }

  /**
   * Lists the connected controllers in the picker.
   *
   * @param {HTMLSelectElement | null} select Picker of the tester, if it has one.
   * @param {Gamepad} gamepad Controller shown, selected in the picker.
   * @param {Gamepad[]} gamepads Connected controllers.
   */
  function fillPicker(select, gamepad, gamepads) {
    if (!select) {
      return;
    }
    select.replaceChildren(...gamepads.map((connected) => new Option(`${connected.index}: ${connected.id}`, connected.index)));
    select.value = String(gamepad.index);
  }

  /**
   * Fills a raw data list with one copy of its template per button or axis.
   *
   * @param {HTMLElement} root Root of the tester.
   * @param {string} kind 'buttons' or 'axes', the value of data-gt-raw and data-gt-template.
   * @param {number} count Number of buttons or axes of the controller.
   */
  function fillRawList(root, kind, count) {
    const list = root.querySelector(`[data-gt-raw="${kind}"]`);
    const template = root.querySelector(`template[data-gt-template="${kind}"]`);
    if (!list || !template) {
      return;
    }

    const items = [];
    for (let index = 0; index < count; index++) {
      const item = template.content.cloneNode(true);
      for (const attribute of ['gtButton', 'gtAxis', 'gtButtonValue', 'gtAxisValue']) {
        for (const element of item.querySelectorAll(`[data-${toKebabCase(attribute)}=""]`)) {
          element.dataset[attribute] = index;
        }
      }
      for (const label of item.querySelectorAll('[data-gt-label]')) {
        label.textContent = label.textContent.replace(INDEX_PLACEHOLDER, index);
      }
      items.push(item);
    }
    list.replaceChildren(...items);
  }

  /**
   * Converts the name of a dataset property to the name of its attribute.
   *
   * @param {string} name Name of the property, e.g. 'gtButtonValue'.
   * @returns {string} Name of the attribute without its data- prefix, e.g. 'gt-button-value'.
   */
  function toKebabCase(name) {
    return name.replace(/[A-Z]/g, (letter) => `-${letter.toLowerCase()}`);
  }

  /**
   * Finds the elements showing the values of the controller, so they aren't searched every frame.
   *
   * @param {HTMLElement} root Root of the tester.
   * @returns {object} The elements, by kind.
   */
  function collectHooks(root) {
    const withIndex = (attribute) =>
      [...root.querySelectorAll(`[data-${toKebabCase(attribute)}]`)]
        .map((element) => ({ element, index: Number(element.dataset[attribute]) }))
        .filter((hook) => Number.isInteger(hook.index));
    return {
      buttons: withIndex('gtButton'),
      axes: withIndex('gtAxis'),
      buttonValues: withIndex('gtButtonValue'),
      axisValues: withIndex('gtAxisValue'),
      sticks: [...root.querySelectorAll('[data-gt-stick]')].map((element) => {
        const [xIndex, yIndex] = element.dataset.gtStick.split(/\s+/).map(Number);
        return { element, xIndex, yIndex };
      }),
    };
  }

  /**
   * Shows the current values of the buttons and axes of a controller.
   *
   * @param {object} tester Tester to update (see startTester()).
   * @param {Gamepad} gamepad Controller shown.
   */
  function updateValues(tester, gamepad) {
    const { hooks } = tester;
    // Writes to the page only when the shown value changes.
    const show = (element, value, write) => {
      if (tester.shownValues.get(element) !== value) {
        tester.shownValues.set(element, value);
        write(value);
      }
    };
    const buttonValue = (index) => formatValue(gamepad.buttons[index]?.value);
    const axisValue = (index) => formatValue(gamepad.axes[index]);

    for (const { element, index } of hooks.buttons) {
      const button = gamepad.buttons[index];
      const value = button ? `${button.pressed} ${buttonValue(index)}` : '';
      show(element, value, () => {
        element.toggleAttribute('data-pressed', Boolean(button?.pressed));
        element.style.setProperty('--gt-value', button ? buttonValue(index) : '0');
      });
    }
    for (const { element, index } of hooks.axes) {
      show(element, axisValue(index), (value) => element.style.setProperty('--gt-value', value || '0'));
    }
    for (const { element, index } of hooks.buttonValues) {
      show(element, buttonValue(index), (value) => (element.textContent = value));
    }
    for (const { element, index } of hooks.axisValues) {
      show(element, axisValue(index), (value) => (element.textContent = value));
    }
    for (const { element, xIndex, yIndex } of hooks.sticks) {
      show(element, `${axisValue(xIndex)} ${axisValue(yIndex)}`, () => {
        element.style.setProperty('--gt-x', axisValue(xIndex) || '0');
        element.style.setProperty('--gt-y', axisValue(yIndex) || '0');
      });
    }
  }

  /**
   * Formats the value of a button or an axis as text.
   *
   * @param {number | undefined} value Value to format.
   * @returns {string} The value with VALUE_DECIMALS decimals, or an empty string if the controller
   *   has no such button or axis.
   */
  function formatValue(value) {
    return typeof value === 'number' ? value.toFixed(VALUE_DECIMALS) : '';
  }

  // #endregion

  // #region Rumble

  /**
   * Wires the rumble inputs and buttons of a tester.
   *
   * @param {object} tester Tester to wire (see startTester()).
   */
  function wireRumble(tester) {
    const { root } = tester;
    for (const input of root.querySelectorAll('[data-gt-rumble-input]')) {
      const output = root.querySelector(`[data-gt-rumble-output="${input.dataset.gtRumbleInput}"]`);
      input.addEventListener('input', () => {
        if (output) {
          output.textContent = input.value;
        }
      });
    }
    root.querySelector('[data-gt-rumble-play]')?.addEventListener('click', () => playRumble(tester));
    root.querySelector('[data-gt-rumble-stop]')?.addEventListener('click', () => stopRumble(tester));
  }

  /**
   * Reads the parameters of the rumble from the inputs of a tester.
   *
   * @param {HTMLElement} root Root of the tester.
   * @returns {GamepadEffectParameters} The parameters of the RUMBLE_EFFECT. The inputs of the
   *   magnitudes go from 0 to 100 %, and the one of the duration is in milliseconds.
   */
  function readRumbleParameters(root) {
    const read = (parameter, fallback) => {
      const input = root.querySelector(`[data-gt-rumble-input="${parameter}"]`);
      return input ? Number(input.value) : fallback;
    };
    return {
      startDelay: 0,
      duration: read('duration', DEFAULT_RUMBLE_DURATION_MS),
      strongMagnitude: read('strongMagnitude', 100) / 100,
      weakMagnitude: read('weakMagnitude', 100) / 100,
    };
  }

  /**
   * Gets the actuator of the controller shown by a tester.
   *
   * @param {object} tester Tester whose controller to get (see startTester()).
   * @returns {GamepadHapticActuator | null} The actuator, or null if no controller is shown or it
   *   can't rumble.
   */
  function getShownActuator(tester) {
    if (tester.shownIndex === null) {
      return null;
    }
    // A fresh object: some browsers give a snapshot of the controller, frozen when it was read.
    const gamepad = navigator.getGamepads()[tester.shownIndex];
    return gamepad && canRumble(gamepad) ? gamepad.vibrationActuator : null;
  }

  /**
   * Makes the controller shown by a tester rumble, with the parameters of its inputs.
   *
   * @param {object} tester Tester whose controller to rumble (see startTester()).
   * @returns {Promise<void>} Resolved once the rumble ends, or is replaced by another one.
   */
  async function playRumble(tester) {
    const actuator = getShownActuator(tester);
    if (!actuator) {
      return;
    }
    try {
      await actuator.playEffect(RUMBLE_EFFECT, readRumbleParameters(tester.root));
    } catch (error) {
      console.warn('Controller tester: the rumble failed.', error);
    }
  }

  /**
   * Stops the rumble of the controller shown by a tester.
   *
   * @param {object} tester Tester whose controller to stop (see startTester()).
   * @returns {Promise<void>} Resolved once the rumble is stopped.
   */
  async function stopRumble(tester) {
    const actuator = getShownActuator(tester);
    if (!actuator) {
      return;
    }
    try {
      await actuator.reset();
    } catch (error) {
      console.warn('Controller tester: the rumble couldn\'t be stopped.', error);
    }
  }

  // #endregion

  // document$ is only defined by the theme. Without it, the page is set up once, like any page.
  if (window.document$) {
    window.document$.subscribe(setupTesters);
  }
  else if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', setupTesters);
  }
  else {
    setupTesters();
  }
})();
