module CreateGame: ((Types.GameLoop, Types.World) => (Types.Game)) = (GL: Types.GameLoop, W: Types.World) => {
    
  open Rationale;
  open Level;
  open Types;

  let initPlayer = name => {name: name, stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (6, 6)};

  let initgame = pname => initPlayer(pname) |> p => {
      player: p,
      world: W.create(p),
      turn: 0.
      };

  let create = name => initgame(name);

  let movePlayer = (x, y, game) => {

    let level = W.currentLevel(game.world);

    level |> 
      Option.bind(_, l =>
          l.map |> Area.movePlayer(x, y, 1.) |>
              Option.ofResult |> o => switch o {
                  | Some(area) => Some(area)
                  | None => None
                  }
              |> Option.fmap((pa:playerArea) => {
                  let xx = { ... l, map: pa.area};

                  let world = W.updateLevel(xx, game.world);

                  {... game, player: pa.player, world: world };
              }))
      |> Option.fmap(GL.continue);
  };
};
