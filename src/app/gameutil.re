open Types;
open Rationale.Option;

module GameUtil = (P:Places, W:World) => {
  let updateCurrentLevel = (area, game) => 
    W.currentLevel(game.world) 
      |> fmap(lvl => { ... lvl, map: area })
      |> fmap(lvl => W.updateLevel(lvl, game.world));

  let attemptUpdate = (area, game) =>
    P.findPlayer(area) 
      |> fmap(player => { ... game, player: player })
      >>= gam => updateCurrentLevel(area, gam) <$> (world => { ... gam, world: world });

  let update = (area, game) => attemptUpdate(area, game) |> default(game);
};
