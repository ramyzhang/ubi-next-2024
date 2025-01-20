#include "stdafx.h"
#include "SSpawner.h"

void SSpawner::InstantiateFloor(const EntityID& _eid,
	Vector3 _colour,
	Vector3 _position,
	Vector3 _scale) {
	std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

	CMesh floor_mesh = {};
	floor_mesh.model = cube_model;
	floor_mesh.colour = _colour;
	SEntityManager::Instance().AddComponent<CMesh>(_eid, floor_mesh);

	CRigidBody floor_rb = {};
	floor_rb.velocity = Vector3(0, 0, 0);
	floor_rb.is_static = true;
	floor_rb.mass = 50.0f;
	SEntityManager::Instance().AddComponent<CRigidBody>(_eid, floor_rb);

	CTransform floor_trans = {};
	floor_trans.rotation = Vector3(0, 0, 0);
	floor_trans.position = _position;
	floor_trans.scale = _scale;
	SEntityManager::Instance().AddComponent<CTransform>(_eid, floor_trans); // floor

	CCollider floor_collider = {};
	floor_collider.center = floor_trans.position;
	floor_collider.volume_type = AABB;
	floor_collider.half_size = floor_trans.scale; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(_eid, floor_collider);
}

void SSpawner::InstantiateCube(
	const EntityID& _eid,
	Vector3 _colour,
	Vector3 _position,
	Vector3 _scale,
	bool _is_static) {
	std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

	CMesh cube_mesh = {};
	cube_mesh.model = cube_model;
	cube_mesh.colour = _colour;
	SEntityManager::Instance().AddComponent<CMesh>(_eid, cube_mesh);

	CTransform cube_trans = {};
	cube_trans.rotation = Vector3(0, 0, 0);
	cube_trans.position = _position;
	cube_trans.scale = _scale;
	SEntityManager::Instance().AddComponent<CTransform>(_eid, cube_trans); // front face

	CRigidBody cube_rb = {};
	cube_rb.velocity = Vector3(0, 0, 0);
	cube_rb.mass = 1.0f;
	cube_rb.is_static = _is_static;
	cube_rb.has_gravity = false;
	SEntityManager::Instance().AddComponent<CRigidBody>(_eid, cube_rb);

	CCollider cube_collider = {};
	cube_collider.center = cube_trans.position;
	cube_collider.volume_type = AABB;
	cube_collider.half_size = cube_trans.scale; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(_eid, cube_collider);
}

void SSpawner::InstantiateBound(
	const EntityID& _eid,
	Vector3 _position,
	Vector3 _scale) {
	CTransform cube_trans = {};
	cube_trans.rotation = Vector3(0, 0, 0);
	cube_trans.position = _position;
	cube_trans.scale = _scale;
	SEntityManager::Instance().AddComponent<CTransform>(_eid, cube_trans); // front face

	CRigidBody cube_rb = {};
	cube_rb.velocity = Vector3(0, 0, 0);
	cube_rb.is_static = true;
	cube_rb.mass = 50.0f;
	SEntityManager::Instance().AddComponent<CRigidBody>(_eid, cube_rb);

	CCollider cube_collider = {};
	cube_collider.center = cube_trans.position;
	cube_collider.volume_type = AABB;
	cube_collider.half_size = cube_trans.scale; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(_eid, cube_collider);
}

void SSpawner::InstantiateBoid(
	const EntityID& _eid,
	Vector3 _position) {
	std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

	CMesh boid_mesh = {};
	boid_mesh.model = cube_model;
	boid_mesh.colour = Vector3(RandomFloat(0.2f, 0.4f), RandomFloat(0.5f, 0.7f), RandomFloat(0.06f, 0.8f));
	SEntityManager::Instance().AddComponent<CMesh>(_eid, boid_mesh);

	CTransform boid_trans = {};
	boid_trans.rotation = Vector3();
	boid_trans.position = _position;
	boid_trans.scale = Vector3(0.5f, 0.5f, 0.5f);
	SEntityManager::Instance().AddComponent<CTransform>(_eid, boid_trans); // front face

	CRigidBody boid_rb = {};
	boid_rb.velocity = Vector3(RandomFloat(0, 1.0f, _eid), RandomFloat(0, 1.0f, _eid), RandomFloat(0, 1.0f, _eid));
	boid_rb.drag = 1.0f;
	boid_rb.is_static = true;
	SEntityManager::Instance().AddComponent<CRigidBody>(_eid, boid_rb);

	CCollider boid_collider = {};
	boid_collider.center = boid_trans.position;
	boid_collider.volume_type = SPHERE;
	boid_collider.radius = 0.25f; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(_eid, boid_collider);

	CBoid cboid = {};
	cboid.forward = boid_rb.velocity;
	SEntityManager::Instance().AddComponent<CBoid>(_eid, cboid);
}

void SSpawner::InstantiatePlayer(const EntityID& _eid) {
	std::shared_ptr<Model> dodec_model = SModelManager::Instance().LoadModel("data/dodecahedron.obj");

	CMesh player_mesh = {};
	player_mesh.model = dodec_model;
	player_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(_eid, player_mesh);

	CTransform player_trans = {};
	player_trans.position = Vector3();
	player_trans.rotation = Vector3();
	player_trans.scale = Vector3(2.0f, 2.0f, 2.0f);
	SEntityManager::Instance().AddComponent<CTransform>(_eid, player_trans); // front face

	CRigidBody player_rb = {};
	player_rb.velocity = Vector3(0, 0, 0);
	player_rb.bounciness = 1.5f; // i wanna be bouncy :D
	player_rb.drag = 0.5f;
	player_rb.is_static = false;
	player_rb.has_gravity = true;
	SEntityManager::Instance().AddComponent<CRigidBody>(_eid, player_rb);

	CCollider player_collider = {};
	player_collider.center = player_trans.position;
	player_collider.volume_type = SPHERE;
	player_collider.radius = (player_trans.scale.x + player_trans.scale.y + player_trans.scale.z) / 3.0f; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(_eid, player_collider);
}

void SSpawner::InstantiatePathDodec(const EntityID& eid, Vector3 _colour, Vector3 _position) {
	std::shared_ptr<Model> dodec_model = SModelManager::Instance().LoadModel("data/dodecahedron.obj");

	CMesh dodec_mesh = {};
	dodec_mesh.model = dodec_model;
	dodec_mesh.colour = _colour;
	SEntityManager::Instance().AddComponent<CMesh>(eid, dodec_mesh);

	CTransform dodec_trans = {};
	dodec_trans.position = _position;
	dodec_trans.rotation = Vector3(0, 0, 0);
	dodec_trans.scale = Vector3(0.5f, 0.5f, 0.5f);
	SEntityManager::Instance().AddComponent<CTransform>(eid, dodec_trans); // front face
}

void SSpawner::InstantiateStar(const EntityID& _eid, Vector3 _position) {
	std::shared_ptr<Model> star_model = SModelManager::Instance().LoadModel("data/star.obj");

	CMesh star_mesh = {};
	star_mesh.model = star_model;
	star_mesh.colour = Vector3(1.0f, 0.725f, 0);
	SEntityManager::Instance().AddComponent<CMesh>(_eid, star_mesh);

	CTransform star_trans = {};
	star_trans.position = _position;
	star_trans.rotation = Vector3();
	star_trans.scale = Vector3(0.01f, 0.01f, 0.01f);
	SEntityManager::Instance().AddComponent<CTransform>(_eid, star_trans); // front face

	CCollider star_collider = {};
	star_collider.center = star_trans.position;
	star_collider.volume_type = SPHERE;
	star_collider.radius = 3.0f;
	SEntityManager::Instance().AddComponent<CCollider>(_eid, star_collider);
}