#ifndef SE_CORE_TOOLS_H_
#define SE_CORE_TOOLS_H_

#include <DirectXMath.h>
namespace SE
{
	namespace Utilz
	{
		/**
		*
		* @brief This class hold general tools that can be reused through out the project.
		* @details The tools created in this library is for common use only, mathematical or other calculations
		* that could be used repeatedly during the project.
		**/

		class Tools
		{
		public:
			/**
			* @breif returns the local matrix of your object.
			* @details Takes the inverse of world and view matrix to calculate your new local matrix.
			* @Param[in] A world matrix.
			* @Param[in] A view matrix.
			* @Param[out] The out matrix.
			**/
			static void GetLocalMatrix(const DirectX::XMMATRIX& worldM, const DirectX::XMMATRIX& invViewM, DirectX::XMMATRIX& out);

			/**
			* @breif Moves your ray from screen space to projection space.
			* @Param[in] Screenspace X-coordinate of your mouse position.
			* @Param[in] Screenspace Y-coordinate of your mouse position.
			* @Param[in] The screen resolution height.
			* @Param[in] The screen resolution width.
			* @Param[out] The out vector.
			**/
			static void RayToView(int sx, int sy, float height, float width, DirectX::XMVECTOR& out);
			/**
			* @breif  Takes your direction ray into local space for picking objects
			* @detail Takes your ray from projection space into local space and calculates the new unit direction vector/normal
			*         of your ray in local space.
			* @Param[in] Your ray direction vector in projection space.
			* @Param[in] Your local matrix.
			* @Param[out] The out vector.
			**/
			static void GetLocalRayDir(const DirectX::XMVECTOR& rayD, const DirectX::XMMATRIX& localM, DirectX::XMVECTOR& out);

			/**
			* @breif  Takes your origin ray into local space for picking objects
			* @detail Takes your ray from projection space into local space.
			* @Param[in] Your ray Origin vector in projection space.
			* @Param[in] Your local matrix.
			* @retval return_value_1 returns your new origin vector.
			**/
			static void GetLocalRayOrigin(const DirectX::XMVECTOR& rayD, const DirectX::XMMATRIX& localM, DirectX::XMVECTOR& out);


		};
	}
}
#endif