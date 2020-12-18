open Utils;

open Types;
open Webapi.Dom;

requireCSS("./App.css");

let string = ReasonReact.string;

type view =
  | Home
  | InGame(Types.game)
  | Score(string, int);

type gameAction =
  | MovePlayer(int, int)
  | TakeStairs
  | UseExit;

let movement = (x, y) => MovePlayer(x, y);

module Game = Modules.Game;

let initPlayer = name => {
  name,
  stats: {
    health: 10,
    speed: 1.0,
    position: 0.,
    damage: 3,
  },
  gold: 0,
  location: (1, 1),
};

[@react.component]
let make = () => {
  let (state: view, updateState) = React.useState(() => Home);
  let (playerName, updateName) = React.useState(() => "");

  let mapGameOrError = (f, view) =>
    switch (view) {
    | InGame(g) => f(g) |> Game.resultUpdateVision
    | _ => Fail("Wrong app state")
    };

  let update = (prev, result): view =>
    switch (result) {
    | Success(game) => InGame(game)
    | EndGame(score, name) => Score(name, score)
    | Fail(error) =>
      Js.Console.error(error);
      prev;
    };

  let handleGameAction = (act, view): view =>
    switch (act) {
    | TakeStairs =>
      view |> mapGameOrError(Game.useStairs(_)) |> update(view)
    | MovePlayer(x, y) =>
      view |> mapGameOrError(Game.movePlayer(x, y)) |> update(view)
    | UseExit => view |> mapGameOrError(Game.useExit) |> update(view)
    };

  let startGame = pname => {
    initPlayer(pname)
    |> (
      p =>
        World.FetchCsvBuilder.create(p)
        |> Js.Promise.then_(w =>
             {player: p, world: w, turn: 0.} |> Js.Promise.resolve
           )
        |> Js.Promise.then_(g => {
            let game = g |> Game.updateVision;
             updateState(_ => InGame(game));
             Js.Promise.resolve(game);
           })
        |> ignore
    );
  };

  let startNewGame = evt =>
    if (ReactEvent.Keyboard.key(evt) == "Enter") {
      startGame(playerName);
    };

  <div className="App">
    {switch (state) {
     | Home => <StartView startGame=startNewGame onChange=updateName />
     | Score(name, score) =>
       <div>
         <div>
           {string(name ++ " scored " ++ string_of_int(score) ++ " points")}
         </div>
         <button onClick={_ => Location.reload(location)}>
           {string("Try again")}
         </button>
       </div>
     | InGame(game) =>
      <GameView
        game
        takeStairs={() => updateState(handleGameAction(TakeStairs))}
        useExit={() => updateState(handleGameAction(UseExit))}
        movePlayer={(x, y) => updateState(handleGameAction(MovePlayer(x, y)))}
      />
     }}
  </div>;
};
