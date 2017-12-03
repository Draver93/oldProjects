#include "direct_mesh.h"
#include "rmxftmpl.h"

///////////////////////////////////////////////////////////////////////////////////////
parser_x::parser_x()
{
	p_direct3d_device = NULL;
	m_texture_path = NULL;
	m_flag = 0;
	m_root_frame = NULL;
	m_root_mesh = NULL;

}
parser_x::~parser_x()
{
	delete m_root_mesh;  
    m_root_mesh = NULL;
	delete m_root_frame; 
	m_root_frame = NULL;
	//free();
}
BOOL parser_x::parse(char *name, void** data)
{
	LPD3DXFILE p_file			= NULL;
	LPD3DXFILEDATA p_file_data	= NULL;
	LPD3DXFILEENUMOBJECT p_enum = NULL;
	//free();

	if (name == NULL) return E_FAIL;
	if (!SUCCEEDED( D3DXFileCreate(&p_file) )) return E_FAIL;
	if (!SUCCEEDED(p_file->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES))){  return E_FAIL; }
	if (!SUCCEEDED(p_file->CreateEnumObject(name, D3DXF_FILELOAD_FROMFILE, &p_enum))){ return E_FAIL; }

	BOOL parse_result = TRUE;
	SIZE_T c_num;
	p_enum->GetChildren(&c_num);
	for (DWORD i = 0; i < c_num; i++)
	{
		p_enum->GetChild(i, &p_file_data);
		parse_result = parse_object(p_file_data, NULL, 0, data, FALSE);
		p_file_data->Release();
	}

	ReleaseCOM(p_enum);
	ReleaseCOM(p_file);
	return TRUE;
}
BOOL parser_x::parse_object(LPD3DXFILEDATA p_data_obj, LPD3DXFILEDATA p_data_ptr_obj, DWORD depth, void** data, BOOL reference)
{
	GUID type = get_obj_guid(p_data_obj);

	if (type == TID_D3DRMMesh && reference == FALSE && m_flag & 1)
	{
		D3DXMESHCONTAINER_EX *mesh = NULL;
		load_mesh(&mesh, p_direct3d_device, p_data_obj, m_texture_path, m_new_FVF, m_load_flags);
		if (mesh)
		{
			mesh->pNextMeshContainer = m_root_mesh;
			m_root_mesh = mesh;
			mesh = NULL;
		}
		if (data)
		{
			D3DXFRAME_EX *p_frame = (D3DXFRAME_EX*)*data;
			if (m_flag & 2 && p_frame) p_frame->pMeshContainer =  m_root_mesh;
		}
	}
	else
	{
		if (data)
		{
			D3DXFRAME_EX *p_frame = (D3DXFRAME_EX*)*data;
			if (m_flag & 2 && m_root_mesh && p_frame)
			{
				char *name = get_obj_name(p_data_obj);
				if (name)
				{
					p_frame->pMeshContainer = m_root_mesh->find(name);
					delete[] name; 
					name = NULL;
				}
			}
		}
	}
	if (type == TID_D3DRMFrame && reference == FALSE && m_flag & 2)
	{
		D3DXFRAME_EX *frame = new D3DXFRAME_EX();
		frame->Name = get_obj_name(p_data_obj);

		if (data == NULL)
		{
			frame->pFrameSibling = m_root_frame;
			m_root_frame = frame;
			frame = NULL;
			data = (void**)&m_root_frame;
		}
		else
		{
			D3DXFRAME_EX *frame_ptr = (D3DXFRAME_EX*)*data;
			frame->pFrameSibling = frame_ptr->pFrameFirstChild;
			frame_ptr->pFrameFirstChild = frame;
			frame = NULL;
			data = (void**)&frame_ptr->pFrameFirstChild;
		}
		frame = NULL;
	}
	if (type == TID_D3DRMFrameTransformMatrix && reference == FALSE && m_flag & 2 && data)
	{
		D3DXFRAME_EX *frame = (D3DXFRAME_EX*)*data;
		if (frame) 
		{
			frame->TransformationMatrix = *(D3DXMATRIX*)get_obj_data(p_data_obj, NULL);
			frame->mat_origin = frame->TransformationMatrix;
		}
	}
	///////////////////////////////
	if (type == TID_D3DRMAnimationSet) // Запись значения в m_anim_set
	{
		c_animation_set *anim_set = new c_animation_set();
		anim_set->m_next = m_anim_set;
		m_anim_set = anim_set;

		m_num_animation_set++;
		anim_set->m_name = get_obj_name(p_data_obj);
	}
	if (type == TID_D3DRMAnimation && m_anim_set) // Создание экземпляра структуры хранящей информ о анимации
	{
		c_animation *anim = new c_animation();
		anim->m_next = m_anim_set->m_animations;
		m_anim_set->m_animations = anim;
		m_anim_set->m_num_animations++;
	}
	if (type == TID_D3DRMFrame && reference == TRUE && m_anim_set && m_anim_set->m_animations) 
	{
		if (p_data_ptr_obj && get_obj_guid(p_data_ptr_obj) == TID_D3DRMAnimation) 
		{
			m_anim_set->m_animations->m_name = get_obj_name(p_data_obj);
		}
		return TRUE;
	}
	if (type == TID_D3DRMAnimationKey && m_anim_set && m_anim_set->m_animations)
	{
		c_animation *anim = m_anim_set->m_animations;
		DWORD *data_ptr = (DWORD*)get_obj_data(p_data_obj, NULL);
		DWORD type_key = *data_ptr++;		//тип 
		DWORD num_keys = *data_ptr++;	//количество ключей 

		switch (type_key)
		{
		case 0: //Вращение
			delete[] anim->m_rotation_keys;
			anim->m_num_rotation_keys = num_keys;
			anim->m_rotation_keys = new c_anim_quat_key[num_keys];

			for (DWORD i = 0; i < num_keys; i++)
			{
				anim->m_rotation_keys[i].m_time = *data_ptr++;
				if (anim->m_rotation_keys[i].m_time > m_anim_set->m_lenght)
				{
					m_anim_set->m_lenght = anim->m_rotation_keys[i].m_time;
				}
				data_ptr++;

				float *f_ptr = (float*)data_ptr;
				anim->m_rotation_keys[i].m_quat_key.w = *f_ptr++;
				anim->m_rotation_keys[i].m_quat_key.x = *f_ptr++;
				anim->m_rotation_keys[i].m_quat_key.y = *f_ptr++;
				anim->m_rotation_keys[i].m_quat_key.z = *f_ptr++;
				data_ptr += 4;
			}
			break;
		case 1: //Размер 
			delete[] anim->m_scale_keys;
			anim->m_num_scale_keys = num_keys;
			anim->m_scale_keys = new c_anim_vec_key[num_keys];

			for (DWORD i = 0; i < num_keys; i++)
			{
				anim->m_scale_keys[i].m_time = *data_ptr++;
				if (anim->m_scale_keys[i].m_time > m_anim_set->m_lenght)
				{
					m_anim_set->m_lenght = anim->m_scale_keys[i].m_time;
				}
				data_ptr++;

				D3DXVECTOR3 *vec_ptr = (D3DXVECTOR3*)data_ptr;
				anim->m_scale_keys[i].m_vec_key = *vec_ptr;
				data_ptr += 3;
			}
			break;
		case 2: //Положение
			delete[] anim->m_translation_keys;
			anim->m_num_translation_keys = num_keys;
			anim->m_translation_keys = new c_anim_vec_key[num_keys];

			for (DWORD i = 0; i < num_keys; i++)
			{
				anim->m_translation_keys[i].m_time = *data_ptr++;
				if (anim->m_translation_keys[i].m_time > m_anim_set->m_lenght)
				{
					m_anim_set->m_lenght = anim->m_translation_keys[i].m_time;
				}

				data_ptr++;

				D3DXVECTOR3 *vec_ptr = (D3DXVECTOR3*)data_ptr;
				anim->m_translation_keys[i].m_vec_key = *vec_ptr;
				data_ptr += 3;
			}
			break;
		case 4: //Матрица преобразования
			delete[] anim->m_matrix_keys;
			anim->m_num_matrix_keys = num_keys;
			anim->m_matrix_keys = new c_anim_matrix_key[num_keys];

			for (DWORD i = 0; i < num_keys; i++)
			{
				anim->m_matrix_keys[i].m_time = *data_ptr++;
				if (anim->m_matrix_keys[i].m_time > m_anim_set->m_lenght)
				{
					m_anim_set->m_lenght = anim->m_matrix_keys[i].m_time;
				}

				data_ptr++;

				D3DXMATRIX *m_ptr = (D3DXMATRIX*)data_ptr;
				anim->m_matrix_keys[i].m_mat_key = *m_ptr;
				data_ptr += 16;
			}
			break;
		}
	}
	return parse_child_object(p_data_obj, depth, data, reference);
}
BOOL parser_x::parse_child_object(LPD3DXFILEDATA p_data_obj, DWORD depth, void** data, BOOL force_reference)
{
	BOOL	parse_result				= TRUE;
	BOOL	b_reference					= TRUE;
	SIZE_T	c_num						= NULL;
	LPD3DXFILEDATA		p_sub_data		= NULL;
	LPD3DXFILEENUMOBJECT	p_sub_obj	= NULL;

	p_data_obj->GetChildren(&c_num);
	for (DWORD i = 0; i < c_num; i++)
	{
		p_data_obj->GetChild(i, &p_sub_data);
		if (p_sub_data->IsReference())
		{
			b_reference = true;
		}
		else
		{
			b_reference = force_reference;
		}
		parse_result = parse_object(p_sub_data, p_data_obj, depth + 1, data, b_reference);
		if (parse_result == FALSE) return FALSE;
	}
	return TRUE;
}

void parser_x::set_animation(DWORD num_anim, c_animation_set *animation)
{
	m_num_animation_set = num_anim;
	m_anim_set = animation;
}
void parser_x::free()
{
	m_num_animation_set = 0;
	delete m_anim_set; 
	m_anim_set = NULL;
}//element ainmation
void parser_x::map_anim(D3DXFRAME_EX *root_frame)
{
	c_animation_set *anim_set = m_anim_set;
	while (anim_set != NULL)
	{
		c_animation *anim = anim_set->m_animations;
		while (anim != NULL)
		{
			anim->m_bone = root_frame->find(anim->m_name);
			anim = anim->m_next;
		}
		anim_set = anim_set->m_next;
	}
}//element ainmation
void parser_x::update_anim(char *anim_set_name, DWORD time, BOOL loop)
{
	c_animation_set *anim_set = m_anim_set;

	if (anim_set_name)
	{
		while (anim_set != NULL)
		{
			if (!strcmp(anim_set->m_name, anim_set_name)) break;
			anim_set = anim_set->m_next;
		}
	}
	if (anim_set == NULL) return;

	if (time > anim_set->m_lenght) time = (loop == TRUE) ? time % (anim_set->m_lenght + 1) : anim_set->m_lenght;

	c_animation *anim = anim_set->m_animations;
	while (anim)
	{
		if (anim->m_bone)
		{
			D3DXMatrixIdentity(&anim->m_bone->TransformationMatrix);
			if (anim->m_num_scale_keys && anim->m_scale_keys)
			{
				DWORD Key1 = 0, Key2 = 0;
				for (DWORD i = 0; i < anim->m_num_scale_keys; i++)
				{
					if (time >= anim->m_scale_keys[i].m_time) Key1 = i;
				}
				Key2 = (Key1 >= (anim->m_num_scale_keys - 1)) ? Key1 : Key1 + 1;

				DWORD TimeDiff = anim->m_scale_keys[Key2].m_time - anim->m_scale_keys[Key1].m_time;
				if (!TimeDiff) TimeDiff = 1;

				float Scalar = (float)(time - anim->m_scale_keys[Key1].m_time) / (float)TimeDiff;

				// Calculate interpolated scale values
				D3DXVECTOR3 vecScale = anim->m_scale_keys[Key2].m_vec_key - anim->m_scale_keys[Key1].m_vec_key;
				vecScale *= Scalar;
				vecScale += anim->m_scale_keys[Key1].m_vec_key;

				// Create scale matrix and combine with transformation
				D3DXMATRIX matScale;
				D3DXMatrixScaling(&matScale, vecScale.x, vecScale.y, vecScale.z);
				anim->m_bone->TransformationMatrix *= matScale;
			}

			// Rotation
			if (anim->m_num_rotation_keys && anim->m_rotation_keys)
			{

				// Loop for matching rotation key
				DWORD Key1 = 0, Key2 = 0;
				for (DWORD i = 0; i < anim->m_num_rotation_keys; i++)
				{
					if (time >= anim->m_rotation_keys[i].m_time) Key1 = i;
				}

				// Get 2nd key number
				Key2 = (Key1 >= (anim->m_num_rotation_keys - 1)) ? Key1 : Key1 + 1;

				// Get difference in keys' times
				DWORD TimeDiff = anim->m_rotation_keys[Key2].m_time - anim->m_rotation_keys[Key1].m_time;
				if (!TimeDiff) TimeDiff = 1;

				// Calculate a scalar value to use
				float Scalar = (float)(time - anim->m_rotation_keys[Key1].m_time) / (float)TimeDiff;

				// slerp rotation values
				D3DXQUATERNION quatRotation;
				D3DXQuaternionSlerp(&quatRotation, &anim->m_rotation_keys[Key1].m_quat_key, &anim->m_rotation_keys[Key2].m_quat_key, Scalar);

				// Create rotation matrix and combine with transformation
				D3DXMATRIX matRotation;
				D3DXMatrixRotationQuaternion(&matRotation, &quatRotation);
				anim->m_bone->TransformationMatrix *= matRotation;
			}

			// Translation
			if (anim->m_num_translation_keys && anim->m_translation_keys)
			{

				// Loop for matching translation key
				DWORD Key1 = 0, Key2 = 0;
				for (DWORD i = 0; i<anim->m_num_translation_keys; i++)
				{
					if (time >= anim->m_translation_keys[i].m_time)
						Key1 = i;
				}

				// Get 2nd key number
				Key2 = (Key1 >= (anim->m_num_translation_keys - 1)) ? Key1 : Key1 + 1;

				// Get difference in keys' times
				DWORD TimeDiff = anim->m_translation_keys[Key2].m_time -
					anim->m_translation_keys[Key1].m_time;
				if (!TimeDiff)
					TimeDiff = 1;

				// Calculate a scalar value to use
				float Scalar = (float)(time - anim->m_translation_keys[Key1].m_time) / (float)TimeDiff;

				// Calculate interpolated vector values
				D3DXVECTOR3 vecPos = anim->m_translation_keys[Key2].m_vec_key -
					anim->m_translation_keys[Key1].m_vec_key;
				vecPos *= Scalar;
				vecPos += anim->m_translation_keys[Key1].m_vec_key;

				// Create translation matrix and combine with transformation
				D3DXMATRIX matTranslation;
				D3DXMatrixTranslation(&matTranslation, vecPos.x, vecPos.y, vecPos.z);
				anim->m_bone->TransformationMatrix *= matTranslation;
			}

			// Matrix
			if (anim->m_num_matrix_keys && anim->m_matrix_keys)
			{
				// Loop for matching matrix key
				DWORD Key1 = 0, Key2 = 0;
				for (DWORD i = 0; i<anim->m_num_matrix_keys; i++)
				{
					if (time >= anim->m_matrix_keys[i].m_time)
						Key1 = i;
				}

				// Get 2nd key number
				Key2 = (Key1 >= (anim->m_num_matrix_keys - 1)) ? Key1 : Key1 + 1;

				// Get difference in keys' times
				DWORD TimeDiff = anim->m_matrix_keys[Key2].m_time -
					anim->m_matrix_keys[Key1].m_time;
				if (!TimeDiff)
					TimeDiff = 1;

				// Calculate a scalar value to use
				float Scalar = (float)(time - anim->m_matrix_keys[Key1].m_time) / (float)TimeDiff;

				// Calculate interpolated matrix
				D3DXMATRIX matDiff = anim->m_matrix_keys[Key2].m_mat_key - anim->m_matrix_keys[Key1].m_mat_key;
				matDiff *= Scalar;
				matDiff += anim->m_matrix_keys[Key1].m_mat_key;

				// Combine with transformation
				anim->m_bone->TransformationMatrix *= matDiff;
			}
		}

		// Go to next animation
		anim = anim->m_next;
	}
}//element ainmation

GUID parser_x::get_obj_guid(LPD3DXFILEDATA p_data_obj)
{
	GUID type;
	if (p_data_obj != NULL) p_data_obj->GetType(&type);
	return type;
}
char *parser_x::get_obj_name(LPD3DXFILEDATA p_data_obj)
{
	char  *name = NULL;
	SIZE_T size = 0;

	if (p_data_obj == NULL) return NULL;
	if (FAILED(p_data_obj->GetName(NULL, &size))) return NULL;
	if (size)
	{
		if ( (name = new char[size]) != NULL )
		{
			p_data_obj->GetName(name, &size);
		}
	}
	return name;
}
void *parser_x::get_obj_data(LPD3DXFILEDATA p_data_obj, DWORD *size)
{
	void *templatedata = NULL;
	SIZE_T templatesize = 0;
	if (p_data_obj == NULL) return NULL;
	if (FAILED(p_data_obj->Lock(&templatesize, (LPCVOID*)&templatedata))) return NULL;
	if (size != NULL) *size = templatesize;
	p_data_obj->Unlock();
	return templatedata;
}
///////////////////////////////////////////////////////////////////////////////////////

HRESULT load_mesh(D3DXMESHCONTAINER_EX **pp_mesh, D3DXFRAME_EX **pp_frame, DWORD *num_animation_set, c_animation_set **anim_set, LPDIRECT3DDEVICE9 p_direct3d_device, char *file_name, char *texture_path, DWORD new_FVF, DWORD load_flags)
{
	parser_x parser;

	if (!p_direct3d_device || !file_name || !texture_path) return E_FAIL;

	parser.p_direct3d_device = p_direct3d_device;
	parser.m_flag = ((!pp_mesh) ? 0 : 1) | ((!pp_frame) ? 0 : 2);
	parser.m_load_flags = load_flags;
	parser.m_new_FVF = new_FVF;
	parser.m_texture_path = texture_path;

	parser.m_root_frame = NULL;
	parser.m_root_mesh	= NULL;



	parser.parse(file_name);

	*num_animation_set = parser.m_num_animation_set;		//element ainmation
	*anim_set = parser.m_anim_set;	//element ainmation

	if (pp_frame && pp_mesh && parser.m_root_frame && parser.m_root_mesh)
	{
		D3DXMESHCONTAINER_EX *p_mesh = parser.m_root_mesh;

		while (p_mesh)
		{
			if (p_mesh->pSkinInfo)
			{
				DWORD num_bones = p_mesh->pSkinInfo->GetNumBones();

				p_mesh->pp_frame_matrices = new D3DXMATRIX*[num_bones];
				p_mesh->p_bone_matrices = new D3DXMATRIX[num_bones];

				for (DWORD i = 0; i < num_bones; i++)
				{
					const char *name = p_mesh->pSkinInfo->GetBoneName(i);
					D3DXFRAME_EX *p_frame = parser.m_root_frame->find(name);
					if (p_frame)
						p_mesh->pp_frame_matrices[i] = &p_frame->mat_combine;
					else
						p_mesh->pp_frame_matrices[i] = NULL;
				}
			}

			p_mesh = (D3DXMESHCONTAINER_EX*)p_mesh->pNextMeshContainer;
		}
	}
	// Copy the pointers into passed variables
	if (pp_mesh) {
		// Assign mesh list pointer
		*pp_mesh = parser.m_root_mesh;
		parser.m_root_mesh = NULL;
	}
	else {
		// Delete list of meshes in case any were loaded
		// and were not needed.
		delete parser.m_root_mesh;
		parser.m_root_mesh = NULL;
	}

	if (pp_frame) {
		// Assign frame hierarchy pointer
		*pp_frame = parser.m_root_frame;
		parser.m_root_frame = NULL;
	}
	else {
		// Delete frame hierarchy in case it was loaded
		// and it was not needed.
		delete parser.m_root_frame;
		parser.m_root_frame = NULL;
	}

	return S_OK;
}

HRESULT load_mesh(D3DXMESHCONTAINER_EX **pp_mesh, LPDIRECT3DDEVICE9 p_direct3d_device, LPD3DXFILEDATA p_data_obj, char *texture_path, DWORD new_FVF, DWORD load_flags)
{
	LPD3DXMESH p_load_mesh	= NULL;
	LPD3DXSKININFO p_skin	= NULL;
	HRESULT hr;

	if (!pp_mesh || !p_direct3d_device || !p_data_obj || !texture_path) return E_FAIL;

	DWORD temp_load_flags = load_flags;
	if (new_FVF) temp_load_flags = D3DXMESH_SYSTEMMEM;

	ID3DXBuffer *material_buffer = NULL, *adjacency_buffer = NULL;
	DWORD num_materials;
	if (FAILED(hr = D3DXLoadSkinMeshFromXof(p_data_obj, temp_load_flags,
	p_direct3d_device, &adjacency_buffer,
	&material_buffer, NULL,
	&num_materials, &p_skin,
	&p_load_mesh)))
	return hr;

	if (p_skin && !p_skin->GetNumBones()) ReleaseCOM(p_skin);
	if (new_FVF)
	{
		ID3DXMesh *p_temp_mesh = NULL;
		if (FAILED(hr = p_load_mesh->CloneMeshFVF(load_flags, new_FVF, p_direct3d_device, &p_temp_mesh)))
		{
			ReleaseCOM(p_load_mesh);
			ReleaseCOM(p_skin);
			ReleaseCOM(material_buffer);
			ReleaseCOM(adjacency_buffer);
			return hr;
		}
		ReleaseCOM(p_load_mesh);
		p_load_mesh = p_temp_mesh;
		p_temp_mesh = NULL;
	}

	// Allocate a D3DXMESHCONTAINER_EX structure
	D3DXMESHCONTAINER_EX *p_mesh = new D3DXMESHCONTAINER_EX();
	*pp_mesh = p_mesh;

	// Store mesh template name, type, and mesh pointers
	SIZE_T Size;
	p_data_obj->GetName(NULL, &Size);
	if (Size)
	{
		p_mesh->Name = new char[Size];
		p_data_obj->GetName(p_mesh->Name, &Size);
	}
	p_mesh->MeshData.Type = D3DXMESHTYPE_MESH;
	p_mesh->MeshData.pMesh = p_load_mesh;
	p_load_mesh = NULL;
	p_mesh->pSkinInfo = p_skin;
	p_skin = NULL;

	// Store adjacency buffer
	DWORD AdjSize = adjacency_buffer->GetBufferSize();
	if (AdjSize) 
	{
		p_mesh->pAdjacency = (DWORD*)new char[AdjSize];
		memcpy(p_mesh->pAdjacency, adjacency_buffer->GetBufferPointer(), AdjSize);
	}
	ReleaseCOM(adjacency_buffer);

	// Create a duplicate mesh in case skinning is used
	if (p_mesh->pSkinInfo)
	p_mesh->MeshData.pMesh->CloneMeshFVF(0, //D3DXMESH_MANAGED,
	p_mesh->MeshData.pMesh->GetFVF(),
	p_direct3d_device, &p_mesh->p_skin_mesh);

	// Build material list
	if (!(p_mesh->NumMaterials = num_materials)) 
	{
		// Create a default material
		p_mesh->NumMaterials = 1;
		p_mesh->pMaterials = new D3DXMATERIAL[1];
		p_mesh->p_textures = new IDirect3DTexture9*[1];

		ZeroMemory(&p_mesh->pMaterials[0], sizeof(D3DXMATERIAL));
		p_mesh->pMaterials[0].MatD3D.Diffuse.r = 1.0f;
		p_mesh->pMaterials[0].MatD3D.Diffuse.g = 1.0f;
		p_mesh->pMaterials[0].MatD3D.Diffuse.b = 1.0f;
		p_mesh->pMaterials[0].MatD3D.Diffuse.a = 1.0f;
		p_mesh->pMaterials[0].MatD3D.Ambient = p_mesh->pMaterials[0].MatD3D.Diffuse;
		p_mesh->pMaterials[0].MatD3D.Specular = p_mesh->pMaterials[0].MatD3D.Diffuse;
		p_mesh->pMaterials[0].pTextureFilename = NULL;
		p_mesh->p_textures[0] = NULL;

	}
	else 
	{

		// Load the materials
		D3DXMATERIAL *materials = (D3DXMATERIAL*)material_buffer->GetBufferPointer();
		p_mesh->pMaterials = new D3DXMATERIAL[p_mesh->NumMaterials];
		p_mesh->p_textures = new IDirect3DTexture9*[p_mesh->NumMaterials];

		for (DWORD i = 0; i<p_mesh->NumMaterials; i++)
		{
			p_mesh->pMaterials[i].MatD3D = materials[i].MatD3D;
			p_mesh->pMaterials[i].MatD3D.Ambient = p_mesh->pMaterials[i].MatD3D.Diffuse;

			// Load the texture if one exists
			p_mesh->p_textures[i] = NULL;
			if (materials[i].pTextureFilename) 
			{
				char TextureFile[MAX_PATH];
				sprintf_s(TextureFile, "%s%s", texture_path,
				materials[i].pTextureFilename);
				D3DXCreateTextureFromFile(p_direct3d_device,
				TextureFile,
				&p_mesh->p_textures[i]);
			}
		}
	}
	ReleaseCOM(material_buffer);

	// Optimize the mesh for better attribute access
	p_mesh->MeshData.pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);

	// Clear pMesh pointer just in case
	p_mesh = NULL;

	return S_OK;
}

HRESULT update_mesh(D3DXMESHCONTAINER_EX *p_mesh)
{
	// Error checking
	if (!p_mesh)
		return E_FAIL;
	if (!p_mesh->MeshData.pMesh || !p_mesh->p_skin_mesh || !p_mesh->pSkinInfo)
		return E_FAIL;
	if (!p_mesh->p_bone_matrices || !p_mesh->pp_frame_matrices)
		return E_FAIL;

	// Copy the bone matrices over (must have been combined before call DrawMesh)
	for (DWORD i = 0; i<p_mesh->pSkinInfo->GetNumBones(); i++) {

		// Start with bone offset matrix
		p_mesh->p_bone_matrices[i] = (*p_mesh->pSkinInfo->GetBoneOffsetMatrix(i));

		// Apply frame transformation
		if (p_mesh->pp_frame_matrices[i])
			p_mesh->p_bone_matrices[i] *= (*p_mesh->pp_frame_matrices[i]);
	}

	// Lock the meshes' vertex buffers
	void *SrcPtr, *DestPtr;
	p_mesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
	p_mesh->p_skin_mesh->LockVertexBuffer(0, (void**)&DestPtr);

	// Update the skinned mesh using provided transformations
	p_mesh->pSkinInfo->UpdateSkinnedMesh(p_mesh->p_bone_matrices, NULL, SrcPtr, DestPtr);

	// Unlock the meshes vertex buffers
	p_mesh->p_skin_mesh->UnlockVertexBuffer();
	p_mesh->MeshData.pMesh->UnlockVertexBuffer();

	// Return success
	return S_OK;
}

HRESULT draw_model(D3DXMESHCONTAINER_EX *p_mesh)
{
	if (!p_mesh && !p_mesh->MeshData.pMesh && (!p_mesh->NumMaterials || !p_mesh->pMaterials)) return E_FAIL;
	
	LPDIRECT3DDEVICE9 pD3DDevice;
	p_mesh->MeshData.pMesh->GetDevice(&pD3DDevice);

	LPD3DXMESH pDrawMesh = (!p_mesh->p_skin_mesh) ? p_mesh->MeshData.pMesh : p_mesh->p_skin_mesh;
	for (DWORD i = 0; i<p_mesh->NumMaterials; i++) 
	{
		//pD3DDevice->SetMaterial(&p_mesh->pMaterials[i].MatD3D);
		//pD3DDevice->SetTexture(0, p_mesh->p_textures[i]);
		pDrawMesh->DrawSubset(i);
	}
	pD3DDevice->Release();
	return S_OK;
}

D3DXMATRIX mesh_transform(LPDIRECT3DDEVICE9 p_direct3d_device, float angle_x, float angle_y, float pos_x, float pos_y, float pos_z, float size)
{
	D3DXMATRIX	matrix_world, 
				matrix_worldX, matrix_worldY, 
				matrix_worldT, 
				matrix_worldS;
	float		angle_rad_x, 
				angle_rad_y;
	angle_rad_x = angle_x*(D3DX_PI / 180);
	angle_rad_y = angle_y*(D3DX_PI / 180);
	D3DXMatrixRotationX(&matrix_worldX, angle_rad_x);
	D3DXMatrixRotationY(&matrix_worldY, angle_rad_y);
	D3DXMatrixMultiply(&matrix_world, &matrix_worldX, &matrix_worldY);

	D3DXMatrixTranslation(&matrix_worldT, pos_x, pos_y, pos_z);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldT);
	D3DXMatrixScaling(&matrix_worldS, size, size, size);
	D3DXMatrixMultiply(&matrix_world, &matrix_world, &matrix_worldS);
	if (!angle_x && !angle_y && !pos_x && !pos_y && !pos_z && !size) D3DXMatrixIdentity(&matrix_world);
	return matrix_world;
}
