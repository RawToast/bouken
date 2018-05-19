open Types;

module World = {
  let updateLevel = (level, world) => { 
    let newLevels = world.levels 
      |> List.map(l => if (level.name == l.name) { level } else { l });
    
    { ... world, levels: newLevels };
  };

  let currentLevel = (world) => world.levels |> Rationale.RList.find(l => l.name == world.current);
};