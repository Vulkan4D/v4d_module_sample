#define _V4D_MODULE
#include <v4d.h>

#include "common.hpp"

v4d::graphics::Window* window = nullptr;
v4d::graphics::Renderer* renderer = nullptr;
v4d::scene::Scene* scene = nullptr;
PlayerView player{};

V4D_MODULE_CLASS(V4D_Input) {
	
	V4D_MODULE_FUNC(std::string, CallbackName) {return THIS_MODULE;}
	
	V4D_MODULE_FUNC(void, Init, v4d::graphics::Window* w, v4d::graphics::Renderer* r, v4d::scene::Scene* s) {
		window = w;
		renderer = r;
		scene = s;
	}
	
	V4D_MODULE_FUNC(void*, ModuleGetCustomPtr, int what) {
		switch (what) {
			case PLAYER: return &player;
		}
		return nullptr;
	}
	
	V4D_MODULE_FUNC(void, CharCallback, unsigned int c) {
		
	}
	
	V4D_MODULE_FUNC(void, KeyCallback, int key, int scancode, int action, int mods) {
		if (action != GLFW_RELEASE
			#ifdef _ENABLE_IMGUI
				&& (!ImGui::IsAnyWindowFocused() || key == GLFW_KEY_ESCAPE)
			#endif
		) {
			std::lock_guard lock(player.mu);
			
			// LOG(scancode) //TODO build platform-specific mapping for scancode when key == -1
			switch (key) {
				
				// Quit
				case GLFW_KEY_ESCAPE:
					glfwSetWindowShouldClose(window->GetHandle(), 1);
					break;
					
				// Reload Renderer
				case GLFW_KEY_R:
					renderer->ReloadRenderer();
					break;
				
				// Increase speed
				case GLFW_KEY_PAGE_UP:
					player.camSpeed *= 10;
					break;
				
				// Decrease speed
				case GLFW_KEY_PAGE_DOWN:
					player.camSpeed *= 0.1;
					break;
				
			}
		}
	}
	
	V4D_MODULE_FUNC(void, ScrollCallback, double x, double y) {
		if (true
			#ifdef _ENABLE_IMGUI
				&& !ImGui::IsAnyWindowFocused()
			#endif
		) {
			std::lock_guard lock(player.mu);
			player.camSpeed *= (1.0+y/10);
		}
	}
	
	V4D_MODULE_FUNC(void, MouseButtonCallback, int button, int action, int mods) {
		if (action == GLFW_RELEASE
			#ifdef _ENABLE_IMGUI
				&& !ImGui::IsAnyWindowFocused()
			#endif
		) {
			switch (button) {
				case GLFW_MOUSE_BUTTON_1:
					glfwSetInputMode(window->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					glfwSetCursorPos(window->GetHandle(), 0, 0);
					break;
				case GLFW_MOUSE_BUTTON_2:
					glfwSetCursorPos(window->GetHandle(), 0, 0);
					glfwSetInputMode(window->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
			}
		}
	}
	
	V4D_MODULE_FUNC(void, Update, double deltaTime) {
		
		// double deltaTime = 0.005f; // No need to calculate it... This seems to already be taken into account in GLFW ???????
		
		// Camera Movements
		double camSpeedMult = glfwGetKey(window->GetHandle(), GLFW_KEY_LEFT_SHIFT)? 4.0 : (glfwGetKey(window->GetHandle(), GLFW_KEY_LEFT_ALT)? 0.25 : 1.0);
		
		std::lock_guard lock(player.mu);
		
		player.velocity = glm::dvec3{0};
		
		if (true
			#ifdef _ENABLE_IMGUI
				&& !ImGui::IsAnyWindowFocused()
			#endif
		) {
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_W)) {
				player.velocity += +player.viewForward * player.camSpeed * camSpeedMult;
			}
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_S)) {
				player.velocity += -player.viewForward * player.camSpeed * camSpeedMult;
			}
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_A)) {
				player.velocity += -player.viewRight * player.camSpeed * camSpeedMult;
			}
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_D)) {
				player.velocity += +player.viewRight * player.camSpeed * camSpeedMult;
			}
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_SPACE)) {
				player.velocity += +player.viewUp * player.camSpeed * camSpeedMult;
			}
			if (glfwGetKey(window->GetHandle(), GLFW_KEY_LEFT_CONTROL)) {
				player.velocity += -player.viewUp * player.camSpeed * camSpeedMult;
			}
		}
		
		player.worldPosition += player.velocity * deltaTime;
		
		if (glfwGetInputMode(window->GetHandle(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED
			#ifdef _ENABLE_IMGUI
				&& !ImGui::IsAnyWindowFocused()
			#endif
		) {
			double x, y;
			glfwGetCursorPos(window->GetHandle(), &x, &y);
			glfwSetCursorPos(window->GetHandle(), 0, 0);
			if (player.useFreeFlyCam) {
				if (x != 0) {
					player.freeFlyCamRotationMatrix = glm::rotate(player.freeFlyCamRotationMatrix, x * player.mouseSensitivity * deltaTime, player.viewUpTarget);
				}
				if (y != 0) {
					player.freeFlyCamRotationMatrix = glm::rotate(player.freeFlyCamRotationMatrix, y * player.mouseSensitivity * deltaTime, player.viewRightTarget);
				}
				if (glfwGetKey(window->GetHandle(), GLFW_KEY_Q)) {
					player.freeFlyCamRotationMatrix = glm::rotate(player.freeFlyCamRotationMatrix, player.tiltSpeed * deltaTime, player.viewForwardTarget);
				}
				if (glfwGetKey(window->GetHandle(), GLFW_KEY_E)) {
					player.freeFlyCamRotationMatrix = glm::rotate(player.freeFlyCamRotationMatrix, -player.tiltSpeed * deltaTime, player.viewForwardTarget);
				}
				player.viewUpTarget = glm::normalize(glm::dvec3(glm::inverse(player.freeFlyCamRotationMatrix) * glm::dvec4(0,0,1, 0)));
				player.viewForwardTarget = glm::normalize(glm::dvec3(glm::inverse(player.freeFlyCamRotationMatrix) * glm::dvec4(0,1,0, 0)));
				player.viewRightTarget = glm::cross(player.viewForwardTarget, player.viewUpTarget);
				if (player.flyCamSmoothness > 2.0) {
					player.viewUp = glm::mix(player.viewUp, player.viewUpTarget, 300.0 / player.flyCamSmoothness * deltaTime);
					player.viewForward = glm::mix(player.viewForward, player.viewForwardTarget, 300.0 / player.flyCamSmoothness * deltaTime);
					player.viewRight = glm::mix(player.viewRight, player.viewRightTarget, 300.0 / player.flyCamSmoothness * deltaTime);
				} else {
					player.viewUp = player.viewUpTarget;
					player.viewForward = player.viewForwardTarget;
					player.viewRight = player.viewRightTarget;
				}
			} else {
				if (x != 0 || y != 0) {
					player.horizontalAngle += double(x * player.mouseSensitivity * deltaTime);
					player.verticalAngle -= double(y * player.mouseSensitivity * deltaTime);
					if (player.verticalAngle < -1.5) player.verticalAngle = -1.5;
					if (player.verticalAngle > 1.5) player.verticalAngle = 1.5;
					player.viewForward = glm::normalize(glm::dvec3(
						cos(player.verticalAngle) * sin(player.horizontalAngle),
						cos(player.verticalAngle) * cos(player.horizontalAngle),
						sin(player.verticalAngle)
					));
					player.viewRight = glm::cross(player.viewForward, player.viewUp);
				}
			}
		}
		
	}
	
};
