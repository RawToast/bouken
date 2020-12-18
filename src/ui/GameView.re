let currentLevel = world =>
  World.currentLevel(world)
  ->Belt_Option.getWithDefault(List.hd(world.levels));
type inxPlace = {
  i: int,
  place: Types.place,
};
type inxRow = {
  i: int,
  place: list(inxPlace),
};

let viewport = (player: Types.player, area) => {
  let size = 6;
  let fullSize = 1 + size * 2;
  let (x, y) = player.location;
  let blocks =
    Belt_List.make(
      size,
      {Types.tile: WALL, state: Empty, tileEffect: NoEff, visible: false},
    );

  let pt1 = area |> List.map(ys => blocks @ ys @ blocks);

  let row =
    pt1
    ->List.hd
    ->List.length
    ->Belt_List.make({
        Types.tile: WALL,
        state: Empty,
        tileEffect: NoEff,
        visible: false,
      })
    ->Belt_List.make(size, _);

  let dropOrEmpty = (list, n) =>
    Belt_List.drop(list, n)->Belt_Option.getWithDefault([]);
  let takeOrEmpty = (list, n) =>
    Belt_List.drop(list, n)->Belt_Option.getWithDefault([]);
  (row @ pt1 @ row)->dropOrEmpty(y)->takeOrEmpty(fullSize)
  |> List.map(l => dropOrEmpty(l, x)->takeOrEmpty(fullSize));
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
