[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;
open GameView;

type route =
  | Home
  | InGame(Types.game)
  | EndGame(string, int);

type actions =
  | MovePlayer(int, int)
  | TakeStairs
  | UseExit
  | ChangeRoute(route);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = (r: option(route)) => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

let ifGameMap = (f, route) => switch(route) {
  | InGame(g) => Some(f(g))
  |  _ => None
};

let ifGameFlatMap = (f, route) => switch(route) {
  | InGame(g) => f(g)
  |  _ => None
};

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);
module Game = Bouken.CreateGame(BasicTurnLoop, World.World);
open Rationale;

let make = (_children) => {
  ...component,
  initialState: () => Home,
  reducer: (act, route: route) =>
    switch act { /* These are returning 'Game' and need to return 'Route' */
    | TakeStairs => route |> ifGameFlatMap(Game.useStairs(_)) |> Rationale.Option.fmap(g => InGame(g)) |> resultToUpdate
    | MovePlayer(x, y) => route |> ifGameFlatMap(Game.movePlayer(x, y)) |> Rationale.Option.fmap(g => InGame(g)) |> resultToUpdate
    | UseExit => route |> ifGameMap(Game.useExit) |> Option.fmap( e => switch e {
      | Types.ContinueGame(game) => InGame(game)
      | Types.EndGame(score) => EndGame("Dave", score)
    }) |> resultToUpdate
    | ChangeRoute(r) => NoUpdate
  },
  render: (self) => {
    <div className="App">
      (switch self.state {
      | Home => (ReasonReact.stringToElement("dave"))
      | EndGame(name, score) => (ReasonReact.stringToElement(name))
      | InGame(game) => 
          <GameView 
            game=(game)
            movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))
            takeStairs=(self.reduce(() => TakeStairs))
            useExit=(self.reduce(() => UseExit))
          />
      })
    </div>
    },
};
