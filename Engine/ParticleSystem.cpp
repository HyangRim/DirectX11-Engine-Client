#include "pch.h"
#include "ParticleSystem.h"
#include "Material.h"
#include "Shader.h"
#include "Windows.h"

ParticleSystem::ParticleSystem() : Super(ComponentType::ParticleSystem)
{
	BuildVB();
}

ParticleSystem::ParticleSystem(shared_ptr<Shader> _shader) : Super(ComponentType::ParticleSystem)
{
	BuildVB();
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Reset()
{
	_firstRun = true;
	_age = 0.0f;
}

void ParticleSystem::Update()
{
	_emitPosW = GetTransform()->GetPosition();
	_timeStep = TIME->GetDeltaTime(); 
	_gameTime = TIME->GetGameTime();

	_age += _timeStep;
}

void ParticleSystem::Render()
{
	Super::Render();

	const shared_ptr<Shader>& shader = m_material->GetShader();
	//
	// Set constants.
	//
	//_fx->SetViewProj(VP);
	//_fx->SetGameTime(_gameTime);
	//_fx->SetTimeStep(_timeStep);
	//_fx->SetEyePosW(_eyePosW);
	//_fx->SetEmitPosW(_emitPosW);
	//_fx->SetEmitDirW(_emitDirW);
	//_fx->SetTexArray(_texArraySRV.Get());
	//_fx->SetRandomTex(_randomTexSRV.Get());

	_desc.timeStep = _timeStep;
	_desc.gameTime = _gameTime;
	_desc.emitDirW = _emitDirW;
	_desc.emitPosW = _emitPosW;
	shader->PushParticleData(_desc);

	//
	// Set IA stage.
	//
	//dc->IASetInputLayout(InputLayouts::Particle.Get());
	//DC->IASetInputLayout(_inputLayout.Get());
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	// the VB that contains the current particle list.	
	uint32 stride = sizeof(ParticleVertex);
	uint32 offset = 0;
	if (_firstRun)
		_initVB->PushData();
	//dc->IASetVertexBuffers(0, 1, _initVB.GetAddressOf(), &stride, &offset);
	//DC->IASetVertexBuffers(0, 1, _initVB->GetComPtr().GetAddressOf(), &stride, &offset);
	else
		_drawVB->PushData();
	//dc->IASetVertexBuffers(0, 1, _drawVB.GetAddressOf(), &stride, &offset);
	//DC->IASetVertexBuffers(0, 1, _drawVB->GetComPtr().GetAddressOf(), &stride, &offset);

//
// Draw the current particle list using stream-out only to update them.  
// The updated vertices are streamed-out to the target VB. 
//
	//uint32 offset = 0;

	shader->BeginDraw(0, 0);
	DC->SOSetTargets(1, _streamOutVB->GetComPtr().GetAddressOf(), &offset);

	//D3DX11_TECHNIQUE_DESC techDesc;
	//shader->Effect()->GetTechniqueByIndex(0)->GetDesc(&techDesc);
	//shader->Effect()->GetTechniqueByIndex(0)->GetPassByIndex(0)->Apply(0, DC.Get());
	if (_firstRun)
	{
		DC->Draw(1, 0);
		_firstRun = false;
	}
	else
	{
		DC->DrawAuto();
	}

	shader->EndDraw(0, 0);
	// ping-pong the vertex buffers
	std::swap(_drawVB, _streamOutVB);
	// done streaming-out--unbind the vertex buffer

	ID3D11Buffer* bufferArray[1] = { 0 };
	//DC->SOSetTargets(1, bufferArray, &offset);

	shader->BeginDraw(1, 0);
	// done streaming-out--unbind the vertex buffer
	//ID3D11Buffer* bufferArray[1] = { 0 };
	DC->SOSetTargets(1, bufferArray, &offset);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	//dc->IASetVertexBuffers(0, 1, _drawVB.GetAddressOf(), &stride, &offset);
	//DC->IASetVertexBuffers(0, 1, _drawVB->GetComPtr().GetAddressOf(), &stride, &offset);
	_drawVB->PushData();

	//shader->Effect()->GetTechniqueByIndex(1)->GetDesc(&techDesc);
	//shader->Effect()->GetTechniqueByIndex(1)->GetPassByIndex(0)->Apply(0, DC.Get());
	DC->DrawAuto();
	shader->EndDraw(1, 0);
}

void ParticleSystem::SetMaterial(shared_ptr<Material> material)
{
	Super::SetMaterial(material);
	material->SetRandomTex(RESOURCES->Get<Texture>(L"RandomTex"));
}

void ParticleSystem::BuildVB()
{
	ParticleVertex p;
	ZeroMemory(&p, sizeof(ParticleVertex));
	p.Age = 0.0f;
	p.Type = 0;

	vector<ParticleVertex> vertices;
	vertices.push_back(p);
	_initVB = make_shared<VertexBuffer>();
	_initVB->Create(vertices, 0, false, true);

	_drawVB = make_shared<VertexBuffer>();
	_drawVB->CreateStreamOut<ParticleVertex>(MAX_PARTICLES);

	_streamOutVB = make_shared<VertexBuffer>();
	_streamOutVB->CreateStreamOut<ParticleVertex>(MAX_PARTICLES);
}
