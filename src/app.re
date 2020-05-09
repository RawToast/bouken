open Utils;

open ReasonReact;
open Types;
open Webapi.Dom;

requireCSS("./App.css");

type view =
  | Home
  | InGame(Types.game)
  | Score(string, int);

type gameAction =
  | MovePlayer(int, int)
  | TakeStairs
  | UseExit;

type appAction =
  | StartGame(string)
  | Begin(game);

type action =
  | GameAction(gameAction)
  | AppAction(appAction);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

module Game = Modules.Game;

let mapGameOrError = (f, view) =>
  switch (view) {
  | InGame(g) => f(g) |> Game.resultUpdateVision
  | _ => Fail("Wrong app state")
  };

let update = result =>
  switch (result) {
  | Success(game) => InGame(game) |> (r => ReasonReact.Update(r))
  | EndGame(score, name) =>
    Score(name, score) |> (r => ReasonReact.Update(r))
  | Fail(error) =>
    Js.Console.error(error);
    ReasonReact.NoUpdate;
  };

let handleGameAction = (act, view) =>
  switch (act) {
  | TakeStairs => view |> mapGameOrError(Game.useStairs(_)) |> update
  | MovePlayer(x, y) =>
    view |> mapGameOrError(Game.movePlayer(x, y)) |> update
  | UseExit => view |> mapGameOrError(Game.useExit) |> update
  };

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

let initgame = (pname, self) =>
  initPlayer(pname)
  |> (
    p =>
      World.FetchCsvBuilder.create(p)
      |> Js.Promise.then_(w =>
           {player: p, world: w, turn: 0.} |> Js.Promise.resolve
         )
      |> Js.Promise.then_(g => {
           self.send(AppAction(Begin(g)));
           Js.Promise.resolve(g);
         })
      |> ignore
  );

let make = _children => {
  ...component,
  initialState: () => Home,
  reducer: (act: action, view) =>
    switch (act) {
    | GameAction(gameAction) => handleGameAction(gameAction, view)
    | AppAction(appAction) =>
      switch (appAction) {
      | StartGame(name) => ReasonReact.SideEffects(initgame(name))
      | Begin(game) => ReasonReact.Update(InGame(game |> Game.updateVision))
      }
    },
  render: self =>
    <div className="App">
      (
        switch (self.state) {
        | Home =>
          <StartView
            startGame=(string => self.send(AppAction(StartGame(string))))
          />
        | Score(name, score) =>
          <div>
            <div>
              (
                ReasonReact.string(
                  name ++ " scored " ++ string_of_int(score) ++ " points",
                )
              )
            </div>
            <button onClick=(_ => Location.reload(location))>
              (string("Try again"))
            </button>
          </div>
        | InGame(game) =>
          <GameView
            game
            takeStairs=(() => self.send(GameAction(TakeStairs)))
            useExit=(() => self.send(GameAction(UseExit)))
            movePlayer=((x, y) => self.send(GameAction(MovePlayer(x, y))))
          />
        }
      )
    </div>,
};
