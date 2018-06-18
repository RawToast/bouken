
open ReasonReact;
open Types;

let currentLevel = (world) => World.World.currentLevel(world) |> Rationale.Option.default(List.hd(world.levels));
type inxPlace = { i: int, place: place };
type inxRow = { i: int, place: list(inxPlace) };

let viewport = (player: player, area) => {
  open Rationale;
  let size = 5;
  let fullSize = 1 + (size * 2);
  let (x, y) = player.location;
  let blocks = RList.repeat({tile: WALL, state: Empty, tileEffect: NoEff }, size);
  
  let pt1 = area |> List.map(ys => blocks @ ys @ blocks);

  let row = pt1 |> List.hd |> List.length |> RList.repeat({tile: WALL, state: Empty, tileEffect: NoEff })
    |> RList.repeat(_, size);

  (row @ pt1 @ row)
    |> RList.drop(y) 
    |> RList.take(fullSize)
    |> List.map(l => RList.drop(x, l) |> RList.take(fullSize));
};

let component = ReasonReact.statelessComponent("GameView");

let make = (~game: game, ~movePlayer, ~takeStairs, ~useExit, _children) => {
  ...component,
  render: (_) => 
    <div>
      <GameStats player=(game.player) turn=(game.turn) level=(game.world.current) />
      <GameMap 
          area=(currentLevel(game.world).map |> viewport(game.player)) 
          movePlayer=(movePlayer)
          takeStairs=(takeStairs)
          useExit=(useExit)
          />
    </div>
};
