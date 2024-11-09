const char* document = R"(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Rock Paper Scissors</title>
    <style>
      * { box-sizing: border-box }

      body {
        font-family: sans-serif;
      }

      .container {
        width: 95%;
        max-width: 1100px;
        margin: 0 auto;
      }

      li + li {
        margin-top: 0.5rem;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Rock Paper Scissors</h1>

      <div id="playbox">
        <p>Select mode:</p>

        <ol>
          <li>
            <button hx-post="/mode-two" hx-target="#playbox" hx-swap="innerHTML">Two players</button>
          </li>

          <li>
            <button hx-post="/mode-one" hx-target="#playbox" hx-swap="innerHTML">Against AI</button>
          </li>

          <li>
            <button hx-post="/mode-zero" hx-target="#playbox" hx-swap="innerHTML">AI against AI</button>
          </li>
      </div>
    </div>

    <script src="/htmx.js" defer></script>
  </body>
</html>
)";

const char* makeMoveHunk = R"(
  <h2>
    __PLAYER_NAME__ makes move:
  </h2>

  <ol>
    <li>
      <button hx-post="/rock" hx-target="#playbox" hx-swap="innerHTML">Rock</button>
    </li>

    <li>
      <button hx-post="/paper" hx-target="#playbox" hx-swap="innerHTML">Paper</button>
    </li>

    <li>
      <button hx-post="/scissors" hx-target="#playbox" hx-swap="innerHTML">Scissors</button>
    </li>
  </ol>
)";

const char* oneAIHunkResults = R"(
  <h2>
    Player 1 made move: __PLAYER_MOVE__<br>
    AI made move: __AI_MOVE__<br>

    __RESULT__
  </h2>

  <button hx-post="/reset" hx-target="#playbox" hx-swap="innerHTML">Play again ?</button>
)";

const char* twoAIsHunkResults = R"(
  <h2>
    AI 1 made move: __AI_1_MOVE__<br>
    AI 2 made move: __AI_2_MOVE__<br>

    __RESULT__
  </h2>

  <button hx-post="/reset" hx-target="#playbox" hx-swap="innerHTML">Play again ?</button>
)";

const char* resultsHunk = R"(
  <h2>
    __RESULT__
  </h2>

  <button hx-post="/reset" hx-target="#playbox" hx-swap="innerHTML">Play again ?</button>
)";

const char* playboxHunk = R"(
  <p>Select mode:</p>

  <ol>
    <li>
      <button hx-post="/mode-two" hx-target="#playbox" hx-swap="innerHTML">Two players</button>
    </li>

    <li>
      <button hx-post="/mode-one" hx-target="#playbox" hx-swap="innerHTML">Against AI</button>
    </li>

    <li>
      <button hx-post="/mode-zero" hx-target="#playbox" hx-swap="innerHTML">AI against AI</button>
    </li>
  </ol>
)";
