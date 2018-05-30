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

module Positions = Positions.BasicPositions;
module BasicTurnLoop = Gameloop.CreateGameLoop(Positions, Gameloop.CreateEnemyLoop(Positions, Level.Area));
module Game = Bouken.CreateGame(BasicTurnLoop, World.World);

let handleGameAction = (act, route) => switch act {
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
          startGame=(string => self.send(AppAction(StartGame(string))))
        />
      | EndGame(name, score) => 
        <div>
          <div>(ReasonReact.string(name ++ " scored " ++ string_of_int(score) ++ " points"))</div>
          <button onClick=(_ => Location.reload(location))>(string("Try again"))</button>
        </div>
      | InGame(game) => 
          <GameView
            game=(game)
            takeStairs=(() => self.send(GameAction(TakeStairs)))
            useExit=(() => self.send(GameAction(UseExit)))
            movePlayer=((x, y) => self.send(GameAction(MovePlayer(x, y))))
          />
      })
    </div>
    },
};
