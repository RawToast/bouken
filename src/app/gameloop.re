open Types;
open Rationale;

module type EnemyLoop = {
  let findActiveEnemies: area => list(enemyInfo);
  let setEnemy: (area, enemyInfo) => option(area);
  let attackablePlaces: (list((int, int)), area) => list((int, int));
  let findTargets: (~range: int=?, Types.enemyInfo) => list((int, int))
};

module CreateEnemyLoop = (Pos: Types.Positions, Places: Types.Places) => {
  let findActiveEnemies = area => 
    area |> List.mapi((xi: int, xs: list(place)) => 
      xs |> List.mapi((yi: int, place: place) => switch place.state {
      | Enemy(e) => if(Pos.isActive(e.stats)){[ { enemy: e, location: (xi, yi) }]} else {[]}
      | _ => []
      } ) |> List.flatten
    ) |> List.flatten;

  let setEnemy = (area, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    Places.setEnemyAt(x, y, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
  };
  let attackablePlaces = (targets, area) => targets 
      |> List.filter(pos => {
        let (x, y) = pos;
        Places.getPlace(x, y, area)
          |> Option.fmap(p => isPlayer(p))
          |> Option.default(false)
      }
    );

  let findTargets = (~range=1, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    let targetX = [(x-range), x, (x+range)] |> List.filter(x => x >= 0);
    let targetY = [(y-range), y, (y+range)] |> List.filter(y => y >= 0);
    targetY |> List.map(y => (targetX |> List.map(x => (x, y)))) |> List.flatten;
  };
};

module CreateGameLoop = (Pos: Types.Positions, EL: EnemyLoop) => {
  open World;

/* This code should be moved to another module */
  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
    game => {
      let activeEnemies: list(enemyInfo) = game.world 
          |> World.currentLevel
          |> Option.fmap(l => EL.findActiveEnemies(l.map))
          |> Option.default([]);

      if (Pos.isActive(game.player.stats)) {
        game;
      } else if (List.length(activeEnemies) >= 1) {
        /* relocate */
        let levelOpt = World.currentLevel(game.world);
        let activeEnemy = List.hd(activeEnemies);
        Js.Console.log(activeEnemy.enemy.name ++ " is active");        

        let canAttack = (~range=1, area, enemyInfo) => {
          let targets = EL.findTargets(~range=range, enemyInfo);
          let attackable = EL.attackablePlaces(targets, area);

          if (List.length(attackable) >= 1) {
            true
          } else {
            false
          };
        };

        let attack = (enemyInfo, area) => {
          let targets = enemyInfo |> EL.findTargets |> EL.attackablePlaces(_, area);
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

        let updatedGame = Option.bind(levelOpt, level => {
          if (canAttack(level.map, activeEnemy)) {
            Js.Console.log(activeEnemy.enemy.name ++ " can attack");

            EL.setEnemy(level.map, activeEnemy) 
              |> Option.bind(_, map => attack(activeEnemy, map))
              |> Option.fmap(r => {
              let (area, player) = r;

              {...level, map: area}
                |> lvl => World.updateLevel(lvl, game.world)
                |> w => {...game, world: w, player: player}
            })
          } else {
            Js.Console.log("is sleeping");
            /* Wait / sleep */
            EL.setEnemy(level.map, activeEnemy) 
            |> Option.fmap(map => {...level, map: map })
            |> Option.fmap(l => World.updateLevel(l, game.world))
            |> Option.fmap(w => {...game, world: w})
          }
        });
        Js.Console.log("is some " ++ string_of_bool(Option.isSome(updatedGame)));

        switch updatedGame {
          | None => game
          | Some(g) => continue(g)
          };
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
