.PHONY: clean All

All:
	@echo "----------Building project:[ RTS-Lab - Debug ]----------"
	@"$(MAKE)" -f  "RTS-Lab.mk" && "$(MAKE)" -f  "RTS-Lab.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ RTS-Lab - Debug ]----------"
	@"$(MAKE)" -f  "RTS-Lab.mk" clean
copy:
	cp ./Debug/RTS-Lab.s19 /mnt/c/Users/axel0/Desktop/Build/bin.s19