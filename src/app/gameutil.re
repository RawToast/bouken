open Types;
module Option = Belt_Option;

module GameUtil = (P:Places, W:World) => {
  let updateCurrentLevel = (area, game) => 
    W.currentLevel(game.world) 
      -> Option.map(lvl => { ... lvl, map: area })
      -> Option.map(lvl => W.updateLevel(lvl, game.world));

  let attemptUpdate = (area, game) =>
    P.findPlayer(area) 
      -> Option.map(player => { ... game, player: player })
      -> Option.flatMap(gam => 
        updateCurrentLevel(area, gam) 
        -> Option.map(world => { ... gam, world: world })
      );

  let update = (area, game) => attemptUpdate(area, game) 
    -> Option.getWithDefault(game);
};
