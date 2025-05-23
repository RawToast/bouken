open Types;
module Option = Belt_Option;

module CreateGameLoop = (Pos: Types.Positions, EL: EnemyLoop) => {
  let loopCost = (1. /. Pos.divisor);
  module World = World.World;
  
  let rec continue: game => game =
    game => {
      let activeEnemies: list(enemyInfo) = game.world 
          -> World.currentLevel
          -> Option.map(l => EL.findActiveEnemies(l.map))
          -> Option.getWithDefault([]);

      if (Pos.isActive(game.player.stats)) {
        game;
      } else if (List.length(activeEnemies) >= 1) {
        /* relocate */
        let levelOpt = World.currentLevel(game.world);
        let activeEnemy = List.hd(activeEnemies);
        let updatedGame = Option.flatMap(levelOpt, level =>
          EL.takeTurn(activeEnemy, level, game)
        );
        updatedGame
        -> Option.mapWithDefault(game, continue);
      } else {
        /* No one is active */
        game.world 
          -> World.currentLevel
          -> Option.map(l => {
            let area = Pos.incrementAll(l.map);
            let world = World.updateLevel( {...l, map: area }, game.world);
            { ...game, world: world, turn: game.turn +. loopCost }
            })
          -> Option.map(g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}})
          -> Option.mapWithDefault(game, continue);
    };
  };
};