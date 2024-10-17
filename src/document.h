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
    </style>
  </head>
  <body>
    <div class="container">
      <span id="counter">counter: 0</span>

      <br>

      <button
        hx-post="/counter-up"
        hx-target="#counter"
        hx-swap="innerHTML"
      >
        +1
      </button>

      <button
        hx-post="/counter-down"
        hx-target="#counter"
        hx-swap="innerHTML"
      >
        -1
      </button>

      <button
        hx-post="/counter-reset"
        hx-target="#counter"
        hx-swap="innerHTML"
      >
        set to 0
      </button>
    </div>
    <script src="/htmx.js"></script>
  </body>
</html>
)";
