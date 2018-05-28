open Types;
open Rationale;

module CreateGameLoop: ((Types.Positions) => (Types.GameLoop)) = (Pos: Types.Positions) => {
  open World;

  type enemyInfo = {
    enemy: enemy,
    position: (int, int)
  };

/* This code should be moved to another module */
let findActiveEnemies = area => 
  area |> List.mapi((xi: int, xs: list(place)) => 
    xs |> List.mapi((yi: int, place: place) => switch place.state {
    | Enemy(e) => if(Pos.isActive(e.stats)){[ { enemy: e, position: (xi, yi) }]} else {[]}
    | _ => []
    } ) |> List.flatten
  ) |> List.flatten;
    
  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
    game => {
      let activeEnemies: list(enemyInfo) = game.world 
          |> World.currentLevel
          |> Option.fmap(l => findActiveEnemies(l.map))
          |> Option.default([]);

      if (Pos.isActive(game.player.stats)) {
        game;
      } else if (List.length(activeEnemies) >= 1) {
        /* relocate */
        let levelOpt = World.currentLevel(game.world);
        let activeEnemy = List.hd(activeEnemies);
        Js.Console.log(activeEnemy.enemy.name ++ " is active");        
        
        let setEnemy = (area, enemyInfo) => {
          let (x, y) = enemyInfo.position;
          Level.Area.setEnemyAt(x, y, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
        };

        let attackablePlaces = (targets, area) => targets 
            |> List.filter(pos => {
              let (x, y) = pos;
              Level.Area.getPlace(x, y, area)
                |> Option.fmap(p => isPlayer(p))
                |> Option.default(false)
            }
          );

        let findTargets = (~range=1, enemyInfo) => {
          let (x, y) = enemyInfo.position;
          let targetX = [(x-range), x, (x+range)] |> List.filter(x => x >= 0);
          let targetY = [(y-range), y, (y+range)] |> List.filter(y => y >= 0);
          targetY |> List.map(y => (targetX |> List.map(x => (x, y)))) |> List.flatten;
        };

        let canAttack = (~range=1, area, enemyInfo) => {
          let targets = findTargets(~range=range, enemyInfo);
          let attackable = attackablePlaces(targets, area);

          if (List.length(attackable) >= 1) {
            true
          } else {
            false
          };
        };

        let attack = (enemyInfo, area) => {
          let targets = enemyInfo |> findTargets |> attackablePlaces(_, area);
          if (List.length(targets) >= 1) {
            let (x,y) = List.hd(targets);
            let place = Level.Area.getPlace(x, y, area);

            Option.bind(place, place =>
                switch place.state {
                  | Player(p) => Some(p)
                  | _ => None;
                })
              |> Option.fmap((player:player) => { ...player, stats: {...player.stats, health: player.stats.health - 1} } )
              |> Option.bind(_, player => Level.Area.setPlayerAt(x, y, player, 0., area) 
                |> Option.ofResult 
                |> Option.fmap(area => (area, player)));
            } else {
            None
          };
        };

        let updatedLevel = Option.bind(levelOpt, level => {
          if (canAttack(level.map, activeEnemy)) {

            setEnemy(level.map, activeEnemy) 
              |> Option.bind(_, map => attack(activeEnemy, map))
               |> Option.fmap(r => {
              let (area, player) = r;

              {...level, map: area}
                |> lvl => World.updateLevel(lvl, game.world)
                |> w => {...game, world: w, player: player}
            })
          } else {
            /* Wait / sleep */
            setEnemy(level.map, activeEnemy) 
            |> Option.fmap(map => {...level, map: map })
            |> Option.fmap(l => World.updateLevel(l, game.world))
            |> Option.fmap(w => {...game, world: w})
          }
        });
        
        Option.default(game, updatedLevel);
      } else {
        /* No one is active */
        let maybeGame = game.world 
          |> World.currentLevel
          |> Rationale.Option.fmap(l => {
            let area = Pos.incrementAll(l.map);
            let world = World.updateLevel( {...l, map: area }, game.world);
            { ...game, world: world, turn: game.turn +. loopCost }
            })
          |> Rationale.Option.fmap(g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}});

        switch maybeGame {
        | None => game
        | Some(g) => continue(g)
        };
    };
  };
};
