kiran
=====

ray-tracer plus photon-mapper for simple offline global illumination

TODO's:
  * properly handle refractions during Russian Roulette
  * remove KD-tree construction artefacts (visible at low photon counts)
  * handle total internal reflections (by actually spawning a reflection ray)
  * make direction probabilities of photons emitted by area lights proportional to cos(angle)
  * support area light sources other than spheres
  * combine anti-aliasing with depth-of-field
  * allow objects to be oriented arbitrarily
  * (triangle) object-meshes and height-fields

