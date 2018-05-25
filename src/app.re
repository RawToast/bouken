[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;
open Rationale;

type route =
  | Home
  | InGame(Types.game)
  | EndGame(string, int);

type gameAction =
  | MovePlayer(int, int)
  | TakeStairs
  | UseExit;

type appAction = 
  | StartGame(string);

type action = 
  | GameAction(gameAction)
  | AppAction(appAction);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = (r: option(route)) => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

let gameToUpdate = g => g |> Option.fmap(g => InGame(g)) |> resultToUpdate;

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

let handleGameAction = (act, route) => switch act { /* These are returning 'Game' and need to return 'Route' */
  | TakeStairs => route |> ifGameFlatMap(Game.useStairs(_)) |> gameToUpdate
  | MovePlayer(x, y) => route |> ifGameFlatMap(Game.movePlayer(x, y)) |> gameToUpdate
  | UseExit => route |> ifGameMap(Game.useExit) |> Option.fmap( e => switch e {
      | ContinueGame(game) => InGame(game)
      | EndGame(score, name) => EndGame(name, score)
    }) |> resultToUpdate
};

open Webapi.Dom;

let make = (_children) => {
  ...component,
  initialState: () => Home,
  reducer: (act: action , route) => switch act {
    | GameAction(gameAction) => handleGameAction(gameAction, route)
    | AppAction(appAction) => switch appAction {
      | StartGame(name) => ReasonReact.Update(InGame(Game.create(name)))
    };
  },
  render: (self) => {
    <div className="App">
      (switch self.state {
      | Home => 
        <StartView 
          startGame=(string => self.reduce(() => AppAction(StartGame(string)))())
        />
      | EndGame(name, score) => 
        <div>
          <div>(ReasonReact.stringToElement(name ++ " scored " ++ string_of_int(score) ++ " points"))</div>
          <button onClick=(_ => Location.reload(location))>(stringToElement("Try again"))</button>
        </div>
      | InGame(game) => 
          <GameView 
            game=(game)
            movePlayer=(x => self.reduce((y) => GameAction(MovePlayer(x, y))))
            takeStairs=(self.reduce(() => GameAction(TakeStairs)))
            useExit=(self.reduce(() => GameAction(UseExit)))
          />
      })
    </div>
    },
};
