[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Types;

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

let resultToUpdate = r => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

let ifGame = (f, route) => switch(route) {
  | InGame(g) => f(g)
  |  _ => NoUpdate
};

module BasicTurnLoop = Gameloop.CreateGameLoop(Positions.BasicPositions);
module Game = Bouken.CreateGame(BasicTurnLoop, World.World);

let make = (_children) => {
  ...component,
  initialState: () => Home,
  reducer: (act, route: route) =>
    switch act { /* These are returning 'Game' and need to return 'Route' */
    | MovePlayer(x, y) => route |> ifGame(Game.movePlayer(x, y) |> resultToUpdate)
    | TakeStairs => route |> ifGame((Game.useStairs |> resultToUpdate))
    | UseExit => route |> ifGame(Game.useExit |> e => switch e {
      | EndGame(score) => NoUpdate
      | ContinueGame(_) => NoUpdate
    })
    | ChangeRoute(r) => ReasonReact.Update(r)
  },
  render: (self) => {
    <div className="App">
/*       
      (switch self.state {
      | Home => (ReasonReact.stringToElement("dave"))
      | EndGame(name, score) => (ReasonReact.stringToElement(name))
      | InGame(game) => 
          <GameView 
            game=(game), 
            movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))
            takeStairs=(self.reduce(() => TakeStairs))
            useExit=(self.reduce(() => UseExit))
          />
      }) */
    </div>
    },
};
