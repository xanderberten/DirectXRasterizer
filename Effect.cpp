#include "pch.h"
#include "Effect.h"
#include <cassert>
#include "Texture.h"

dae::Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile):
m_pEffect{LoadEffect(pDevice, assetFile)}
{
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechniquePoint");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"\nTechnique not valid\n";
		assert(false, "\nTechnique not valid\n");
	}

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if(!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"\nm_pMatWorldViewProjVariable not valid\n";
		assert(false, "\m_pMatWorldViewProjVariable not valid\n");
	}

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid()) 
	{
		std::wcout << L"\nm_pDiffuseMapVariable not valid\n";
		assert(false, "\nm_pDiffuseMapVariable not valid\n");
	}
}

dae::Effect::~Effect()
{
	if(m_pEffect)	m_pEffect->Release();  
	//TODO: Ask if this needs to be released
	//m_pTechnique->Release();
	//m_pDiffuseMapVariable->Release();
}


ID3DX11Effect* dae::Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* dae::Effect::GetTechnique() const
{
	return  m_pTechnique;
}

void dae::Effect::SetWorldViewProjectionMatrix(const float* pData)
{
	m_pMatWorldViewProjVariable->SetMatrix(pData);
}

void dae::Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable) 
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	}
}

void dae::Effect::SetTechnique(const LPCSTR& techniqueName)
{
	m_pTechnique = m_pEffect->GetTechniqueByName(techniqueName);
	std::cout << '\n' << techniqueName << '\n';
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"\nTechnique not valid\n";
		assert(false, "\nTechnique not valid\n");
	}
}

ID3DX11Effect* dae::Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile
	(
		assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob
	);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };

			std::wstringstream ss;
			for (unsigned int i{}; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << "\n";
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << "\n";
			return nullptr;
		}
	}

	return pEffect;
}
