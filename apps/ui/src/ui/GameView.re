let currentLevel = world =>
  BoukenGame.World.currentLevel(world)
  -> Belt_Option.getWithDefault(List.hd(world.levels));

type inxPlace = {
  i: int,
  place: BoukenGame.Types.place,
};
type inxRow = {
  i: int,
  place: list(inxPlace),
};

let viewport = (player: BoukenGame.Types.player, area) => {
  let size = 6;
  let fullSize = 1 + size * 2;
  let (x, y) = player.location;
  let blocks =
    Belt_List.make(
      size,
      {BoukenGame.Types.tile: WALL, state: Empty, tileEffect: NoEff, visible: false},
    );

  let pt1 = area -> Belt_List.map(ys => blocks @ ys @ blocks);

  let row =
    (pt1
    |> List.hd
    |> List.length)
    -> Belt_List.make(_, {
         BoukenGame.Types.tile: WALL,
         state: Empty,
         tileEffect: NoEff,
         visible: false,
       })
    -> Belt_List.make(size, _);

  let ddrop = (l, i) => Belt_List.drop(l, i) -> Belt_Option.getWithDefault([]);
  let ttake = (l, i) => Belt_List.take(l, i) -> Belt_Option.getWithDefault([]);

  ((row
  @ pt1
  @ row)
  -> ddrop(y)
  -> ttake(fullSize))
  |> List.map(l => ddrop(l, x) -> ttake(fullSize));
};

[@react.component]
let make = (~game: BoukenGame.Types.game, ~movePlayer, ~takeStairs, ~useExit) => {
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