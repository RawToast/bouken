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

let mapGameOrError = (f, route) => switch(route) {
  | InGame(g) => f(g)
  |  _ => Error("Wrong app state")
};

let update = (result) => switch result {
  | Ok(game) => InGame(game) |> r => ReasonReact.Update(r)
  | EndGame(score, name) => EndGame(name, score) |> r => ReasonReact.Update(r)
  | Error(error) => Js.Console.error(error); ReasonReact.NoUpdate
};

module Game = Modules.Game;

let handleGameAction = (act, route) => switch act {
  | TakeStairs => route |> mapGameOrError(Game.useStairs(_)) |> update
  | MovePlayer(x, y) => route |> mapGameOrError(Game.movePlayer(x, y)) |> update
  | UseExit => route |> mapGameOrError(Game.useExit) |> update
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
