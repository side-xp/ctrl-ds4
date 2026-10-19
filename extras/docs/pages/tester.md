---
description: Test a CtrlDS4 board from your browser, with the Gamepad API.
---

# Controller tester

Check that your board works as a DS4 controller, right from your browser: plug it in, then press any of its buttons. Open this page on the host you want to use the controller with (a computer, a phone...) to check that this host recognizes it.

<!--
  Structure of the tester, filled in by assets/javascripts/gamepadTester.js (see the list of the
  data-gt-* attributes it uses at the top of that file). The indices of the buttons and axes are
  the ones of the standard mapping of the Gamepad API: https://w3c.github.io/gamepad/#remapping
-->
<div class="gamepad-tester" data-gamepad-tester>
  <noscript>
    <div class="admonition failure">
      <p class="admonition-title">JavaScript required</p>
      <p>The tester needs JavaScript to read the controller.</p>
    </div>
  </noscript>

  <div class="admonition failure" data-gt-when="unsupported">
    <p class="admonition-title">Gamepad API unavailable</p>
    <p>This browser doesn't support the Gamepad API, so it can't read the controller. Try another browser, such as an up-to-date version of Chrome, Edge, Firefox or Safari.</p>
  </div>

  <div class="admonition info" data-gt-when="waiting">
    <p class="admonition-title">Waiting for a controller</p>
    <p>Plug your board in, then press any of its buttons. Browsers only show a controller to a page once one of its buttons is pressed.</p>
  </div>

  <div class="gamepad-tester__info" data-gt-when="connected">
    <label class="gamepad-tester__picker" data-gt-when="several">
      Controller
      <select data-gt-select></select>
    </label>
    <strong data-gt-field="id"></strong>
    <span>Mapping: <code data-gt-field="mapping"></code></span>
    <span><span data-gt-field="buttons"></span> buttons, <span data-gt-field="axes"></span> axes</span>
  </div>

  <div class="admonition warning" data-gt-when="connected nonstandard">
    <p class="admonition-title">Non-standard mapping</p>
    <p>This browser doesn't map the controller to the standard layout of the Gamepad API, so its buttons and axes can't be shown on the controller below. Check them in the raw data instead. Chromium-based browsers (Chrome, Edge...) map a DS4 to the standard layout.</p>
  </div>

  <div class="gamepad-tester__layout" data-gt-when="connected standard">
    <svg class="gamepad-tester__controller" viewBox="0 0 600 380" role="img" aria-label="State of the controls of the controller">
      <path class="gamepad-tester__body" d="M 120 70 Q 160 50 210 52 L 390 52 Q 440 50 480 70 Q 540 95 560 180 Q 585 290 560 340 Q 535 375 495 350 Q 460 325 430 270 L 170 270 Q 140 325 105 350 Q 65 375 40 340 Q 15 290 40 180 Q 60 95 120 70 Z"/>

      <!-- Triggers (L2, R2): analog, so they fill up as they're pressed -->
      <g class="gamepad-tester__control gamepad-tester__trigger" data-gt-button="6">
        <title>L2</title>
        <rect class="gamepad-tester__shape" x="100" y="6" width="105" height="22" rx="6"/>
        <rect class="gamepad-tester__fill" x="100" y="6" width="105" height="22" rx="6"/>
        <text class="gamepad-tester__label" x="152" y="17">L2</text>
      </g>
      <g class="gamepad-tester__control gamepad-tester__trigger" data-gt-button="7">
        <title>R2</title>
        <rect class="gamepad-tester__shape" x="395" y="6" width="105" height="22" rx="6"/>
        <rect class="gamepad-tester__fill" x="395" y="6" width="105" height="22" rx="6"/>
        <text class="gamepad-tester__label" x="448" y="17">R2</text>
      </g>

      <!-- Shoulder buttons (L1, R1) -->
      <g class="gamepad-tester__control" data-gt-button="4">
        <title>L1</title>
        <rect class="gamepad-tester__shape" x="100" y="32" width="105" height="17" rx="8"/>
        <text class="gamepad-tester__label" x="152" y="40.5">L1</text>
      </g>
      <g class="gamepad-tester__control" data-gt-button="5">
        <title>R1</title>
        <rect class="gamepad-tester__shape" x="395" y="32" width="105" height="17" rx="8"/>
        <text class="gamepad-tester__label" x="448" y="40.5">R1</text>
      </g>

      <!-- Touchpad (click only, the Gamepad API doesn't give the fingers), Share and Options -->
      <g class="gamepad-tester__control" data-gt-button="17">
        <title>Touchpad</title>
        <rect class="gamepad-tester__shape" x="210" y="62" width="180" height="100" rx="12"/>
        <text class="gamepad-tester__label" x="300" y="112">Touchpad</text>
      </g>
      <g class="gamepad-tester__control" data-gt-button="8">
        <title>Share</title>
        <rect class="gamepad-tester__shape" x="180" y="72" width="14" height="26" rx="7"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="9">
        <title>Options</title>
        <rect class="gamepad-tester__shape" x="406" y="72" width="14" height="26" rx="7"/>
      </g>

      <!-- D-pad -->
      <path class="gamepad-tester__plate" d="M 114 111 H 146 V 139 H 172 V 171 H 146 V 199 H 114 V 171 H 88 V 139 H 114 Z"/>
      <g class="gamepad-tester__control" data-gt-button="12">
        <title>D-pad up</title>
        <rect class="gamepad-tester__shape" x="118" y="115" width="24" height="26" rx="4"/>
        <path class="gamepad-tester__symbol" d="M 124 132 L 130 124 L 136 132"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="13">
        <title>D-pad down</title>
        <rect class="gamepad-tester__shape" x="118" y="169" width="24" height="26" rx="4"/>
        <path class="gamepad-tester__symbol" d="M 124 178 L 130 186 L 136 178"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="14">
        <title>D-pad left</title>
        <rect class="gamepad-tester__shape" x="92" y="143" width="26" height="24" rx="4"/>
        <path class="gamepad-tester__symbol" d="M 109 149 L 101 155 L 109 161"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="15">
        <title>D-pad right</title>
        <rect class="gamepad-tester__shape" x="142" y="143" width="26" height="24" rx="4"/>
        <path class="gamepad-tester__symbol" d="M 151 149 L 159 155 L 151 161"/>
      </g>

      <!-- Face buttons -->
      <g class="gamepad-tester__control" data-gt-button="3">
        <title>Triangle</title>
        <circle class="gamepad-tester__shape" cx="470" cy="120" r="16"/>
        <path class="gamepad-tester__symbol" d="M 470 112 L 478 126 L 462 126 Z"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="1">
        <title>Circle</title>
        <circle class="gamepad-tester__shape" cx="505" cy="155" r="16"/>
        <circle class="gamepad-tester__symbol" cx="505" cy="155" r="7"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="0">
        <title>Cross</title>
        <circle class="gamepad-tester__shape" cx="470" cy="190" r="16"/>
        <path class="gamepad-tester__symbol" d="M 464 184 L 476 196 M 476 184 L 464 196"/>
      </g>
      <g class="gamepad-tester__control" data-gt-button="2">
        <title>Square</title>
        <circle class="gamepad-tester__shape" cx="435" cy="155" r="16"/>
        <rect class="gamepad-tester__symbol" x="429" y="149" width="12" height="12"/>
      </g>

      <!-- Sticks, moved by their axes, and their clicks (L3, R3) -->
      <g data-gt-stick="0 1">
        <circle class="gamepad-tester__plate" cx="215" cy="230" r="36"/>
        <g class="gamepad-tester__cap">
          <g class="gamepad-tester__control" data-gt-button="10">
            <title>Left stick (L3)</title>
            <circle class="gamepad-tester__shape" cx="215" cy="230" r="24"/>
            <text class="gamepad-tester__label" x="215" y="230">L3</text>
          </g>
        </g>
      </g>
      <g data-gt-stick="2 3">
        <circle class="gamepad-tester__plate" cx="385" cy="230" r="36"/>
        <g class="gamepad-tester__cap">
          <g class="gamepad-tester__control" data-gt-button="11">
            <title>Right stick (R3)</title>
            <circle class="gamepad-tester__shape" cx="385" cy="230" r="24"/>
            <text class="gamepad-tester__label" x="385" y="230">R3</text>
          </g>
        </g>
      </g>

      <!-- PS button -->
      <g class="gamepad-tester__control" data-gt-button="16">
        <title>PS</title>
        <circle class="gamepad-tester__shape" cx="300" cy="230" r="12"/>
        <text class="gamepad-tester__label" x="300" y="230">PS</text>
      </g>
    </svg>

    <table>
      <thead>
        <tr><th>Control</th><th>Value</th></tr>
      </thead>
      <tbody>
        <tr><td>Left stick X</td><td><output data-gt-axis-value="0"></output></td></tr>
        <tr><td>Left stick Y</td><td><output data-gt-axis-value="1"></output></td></tr>
        <tr><td>Right stick X</td><td><output data-gt-axis-value="2"></output></td></tr>
        <tr><td>Right stick Y</td><td><output data-gt-axis-value="3"></output></td></tr>
        <tr><td>L2</td><td><output data-gt-button-value="6"></output></td></tr>
        <tr><td>R2</td><td><output data-gt-button-value="7"></output></td></tr>
      </tbody>
    </table>
  </div>

  <fieldset class="gamepad-tester__rumble" data-gt-when="connected">
    <legend>Rumble</legend>
    <p data-gt-when="no-rumble">This browser can't make the controller rumble. Try a Chromium-based browser (Chrome, Edge...).</p>
    <div data-gt-when="rumble">
      <p>Sends a rumble to the board, which receives it like from a game: see the <a href="https://github.com/side-xp/ctrl-ds4/blob/HEAD/examples/RumbleAndLightbar/RumbleAndLightbar.ino">RumbleAndLightbar</a> example to print it to the Serial Monitor.</p>
      <label class="gamepad-tester__slider">
        <span>Strong motor (left)</span>
        <input type="range" min="0" max="100" step="5" value="100" data-gt-rumble-input="strongMagnitude">
        <span><output data-gt-rumble-output="strongMagnitude">100</output> %</span>
      </label>
      <label class="gamepad-tester__slider">
        <span>Weak motor (right)</span>
        <input type="range" min="0" max="100" step="5" value="100" data-gt-rumble-input="weakMagnitude">
        <span><output data-gt-rumble-output="weakMagnitude">100</output> %</span>
      </label>
      <!-- Chromium-based browsers refuse effects longer than 5 seconds -->
      <label class="gamepad-tester__slider">
        <span>Duration</span>
        <input type="range" min="100" max="5000" step="100" value="1000" data-gt-rumble-input="duration">
        <span><output data-gt-rumble-output="duration">1000</output> ms</span>
      </label>
      <div class="gamepad-tester__actions">
        <button type="button" class="md-button md-button--primary" data-gt-rumble-play>Rumble</button>
        <button type="button" class="md-button" data-gt-rumble-stop>Stop</button>
      </div>
    </div>
  </fieldset>

  <details class="note" data-gt-when="connected" data-gt-open-when="nonstandard">
    <summary>Raw data</summary>
    <p>Every button and axis of the controller, as the browser gives them. The Y axes go from up (-1) to down (1), the opposite of <code>CtrlDS4::StickAxis</code>.</p>
    <p><strong>Buttons</strong></p>
    <div class="gamepad-tester__raw-list" data-gt-raw="buttons"></div>
    <p><strong>Axes</strong></p>
    <div class="gamepad-tester__raw-list" data-gt-raw="axes"></div>
    <template data-gt-template="buttons">
      <div class="gamepad-tester__raw-item" data-gt-button>
        <span data-gt-label>Button {index}</span>
        <output data-gt-button-value></output>
        <span class="gamepad-tester__bar"></span>
      </div>
    </template>
    <template data-gt-template="axes">
      <div class="gamepad-tester__raw-item" data-gt-axis>
        <span data-gt-label>Axis {index}</span>
        <output data-gt-axis-value></output>
        <span class="gamepad-tester__bar gamepad-tester__bar--axis"></span>
      </div>
    </template>
  </details>
</div>

## If the controller isn't detected

- **Press a button:** browsers only show a controller to a page once one of its buttons is pressed, even if it was plugged in before the page was opened.
- **Check the board:** if the host doesn't see the board at all, see the [troubleshooting](index.md#troubleshooting) section.

## What this page can't test

The Gamepad API only gives the buttons and the axes of a controller, and its rumble in some browsers. So these parts of the DS4 can't be checked here:

- **Lightbar:** browsers can't change its color.
- **Touchpad fingers and motion sensors:** browsers don't give them (and the library doesn't emulate them yet, see the [known limitations](index.md#known-limitations)). The touchpad click is shown, as a button.
