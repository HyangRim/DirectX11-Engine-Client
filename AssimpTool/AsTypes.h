#pragma once


using VertexType = VertexTextureNormalTangentBlendData;

struct asBone {
	string m_name;

	int32 m_index = -1;
	int32 m_parent = -1;

	Matrix m_transform;
};

struct asMesh
{
	string m_name;
	aiMesh* m_mesh;
	vector<VertexType> m_vertices;
	vector<uint32> m_indices;

	int32 m_boneIndex;
	string m_materialName;
};

struct asMaterial
{
	string m_name;
	Color m_ambient;
	Color m_diffuse;
	Color m_specular;
	Color m_emissive;

	string m_diffuseFile;
	string m_specularFile;
	string m_normalFile;
};


//Animation

struct asBlendWeight {

	Vec4 m_indices = Vec4(0, 0, 0, 0);
	Vec4 m_weights = Vec4(0, 0, 0, 0);

	void Set(uint32 index, uint32 boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
			case 0: m_indices.x = i; m_weights.x = w; break;
			case 1: m_indices.y = i; m_weights.y = w; break;
			case 2: m_indices.z = i; m_weights.z = w; break;
			case 3: m_indices.w = i; m_weights.w = w; break;
		}
	}

};

// 정점마다 -> (관절번호, 가중치)
struct asBoneWeights {

	using Pair = pair<int32, float>;
	vector<Pair> m_boneWeights;

	void AddWeights(uint32 _boneIndex, float _weight)
	{
		if (_weight <= 0.0f)
			return;

		//가중치가 더 작은 애가 있는지 확인. 
		auto findIt = std::find_if(m_boneWeights.begin(), m_boneWeights.end(),
			[_weight](const Pair& p) { return _weight > p.second; });

		//(1, 0.4) (2, 0.2) 비율이 큰 순서대로 앞에 놓음. 
		//가중치가 높은 것들을 앞에 놓음. 
		m_boneWeights.insert(findIt, Pair(_boneIndex, _weight));
	}

	asBlendWeight GetBlendWeights()
	{
		asBlendWeight blendWeights;

		for (uint32 i = 0; i < m_boneWeights.size(); i++)
		{
			if (i >= 4)
				break;

			blendWeights.Set(i, m_boneWeights[i].first, m_boneWeights[i].second);
		}

		return blendWeights;
	}

	void Normalize()
	{
		if (m_boneWeights.size() >= 4)
			m_boneWeights.resize(4);

		float totalWeight = 0.f;
		for (const auto& item : m_boneWeights)
			totalWeight += item.second;

		float scale = 1.f / totalWeight;
		for (auto& item : m_boneWeights)
			item.second *= scale;
	}


};
struct asKeyFrameData {
	float m_time;
	Vec3 m_scale;
	Quaternion m_rotation;
	Vec3 m_translation;
};

struct asKeyFrame {
	string m_boneName;
	vector<asKeyFrameData> transform;
};


struct asAnimation {
	string m_name;
	uint32 m_frameCount;
	float m_frameRate;
	float m_duration;
	vector<shared_ptr<asKeyFrame>> m_keyFrames;
};

struct asAnimationnode {
	aiString m_name;
	vector<asKeyFrameData> m_keyFrame;
};