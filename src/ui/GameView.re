let currentLevel = world =>
  World.currentLevel(world)
  |> Rationale.Option.default(List.hd(world.levels));
type inxPlace = {
  i: int,
  place: Types.place,
};
type inxRow = {
  i: int,
  place: list(inxPlace),
};

let viewport = (player: Types.player, area) => {
  open Rationale;
  let size = 6;
  let fullSize = 1 + size * 2;
  let (x, y) = player.location;
  let blocks =
    RList.repeat(
      {Types.tile: WALL, state: Empty, tileEffect: NoEff, visible: false},
      size,
    );

  let pt1 = area |> List.map(ys => blocks @ ys @ blocks);

  let row =
    pt1
    |> List.hd
    |> List.length
    |> RList.repeat({
         Types.tile: WALL,
         state: Empty,
         tileEffect: NoEff,
         visible: false,
       })
    |> RList.repeat(_, size);

  row
  @ pt1
  @ row
  |> RList.drop(y)
  |> RList.take(fullSize)
  |> List.map(l => RList.drop(x, l) |> RList.take(fullSize));
};

[@react.component]
let make = (~game: Types.game, ~movePlayer, ~takeStairs, ~useExit) => {
  <div>
    <GameStats
      player={game.player}
      turn={game.turn}
      level={game.world.current}
    />
    <GameMap
      area={currentLevel(game.world).map |> viewport(game.player)}
      movePlayer
      takeStairs
      useExit
    />
  </div>;
};
