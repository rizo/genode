#
# Configuration variables for the platform-specific run scripts:
#
# $platform          - name of the platform passed to the 'create_builddir' script
# $platform_base_dir - name of the platform-specific Genode base directory
# $platform_cmds     - platform-specific shell commands to be executed at startup
# $platform_pkgs     - names of platform-specific tar archives without the '.tar' extension
#

set build_components {
	core init drivers/timer noux/minimal lib/libc_noux
	drivers/framebuffer drivers/pci drivers/input
	server/terminal server/ram_fs
	test/libports/ncurses
}

set cc_march ""

if {[have_spec arm]} {
	set binutils "binutils_arm"
	set gcc "gcc_arm"
	set tool_prefix "genode-arm-"
}

if {[have_spec x86]} {
	set binutils "binutils_x86"
	set gcc "gcc_x86"
	set tool_prefix "genode-x86-"
	if {[have_spec x86_32]} {
		set cc_march "-m32"
        }
}

#
# Build Noux packages only once
#
set noux_pkgs "bash coreutils vim findutils make which $binutils $gcc"

foreach pkg $noux_pkgs {
	lappend_if [expr ![file exists bin/$pkg]] build_components noux-pkg/$pkg }

build $build_components

# write default vimrc file
set vimrc_fd [open "bin/vim/share/vim/vimrc" w]
puts $vimrc_fd {
set noloadplugins
set hls
set nocompatible
set laststatus=2
set noswapfile
set viminfo=}
close $vimrc_fd

# strip all binaries prior archiving
set find_args ""
foreach pkg $noux_pkgs { append find_args " bin/$pkg" }
exec sh -c "find $find_args -type f | (xargs [cross_dev_prefix]strip -g || true) 2>/dev/null"

exec ln -sf bash bin/bash/bin/sh

foreach pkg $noux_pkgs {
	exec tar cfv bin/$pkg.tar -h -C bin/$pkg . }

set genode_repositories "tool base $platform_base_dir os demo"
foreach rep $genode_repositories { append genode_repositories_filter_out_find " -not -name $rep " }
set genode_repositories_filter_out [exec sh -c "find $genode_dir -mindepth 1 -maxdepth 1 $genode_repositories_filter_out_find -printf \"%f \""]
foreach rep $genode_repositories_filter_out { append genode_repositories_filter_out_tar " --exclude=./$rep " }
set tar_command "tar cfv bin/genode.tar $genode_repositories_filter_out_tar -C $genode_dir ."
eval exec $tar_command

create_boot_directory

append config {
	<config verbose="yes">
		<parent-provides>
			<service name="ROM"/>
			<service name="LOG"/>
			<service name="CAP"/>
			<service name="RAM"/>
			<service name="RM"/>
			<service name="CPU"/>
			<service name="PD"/>
			<service name="IRQ"/>
			<service name="IO_PORT"/>
			<service name="IO_MEM"/>
			<service name="SIGNAL"/>
		</parent-provides>
		<default-route>
			<any-service> <any-child/> <parent/> </any-service>
		</default-route>
		<start name="timer">
			<resource name="RAM" quantum="1M"/>
			<provides><service name="Timer"/></provides>
		</start> }

append_if [have_spec sdl] config {
	<start name="fb_sdl">
		<resource name="RAM" quantum="4M"/>
		<provides>
			<service name="Input"/>
			<service name="Framebuffer"/>
		</provides>
	</start>}

append_if [have_spec pci] config {
	<start name="pci_drv">
		<resource name="RAM" quantum="1M"/>
		<provides><service name="PCI"/></provides>
	</start>}

append_if [have_spec vesa] config {
	<start name="vesa_drv">
		<resource name="RAM" quantum="1M"/>
		<provides><service name="Framebuffer"/></provides>
	</start>}

append_if [have_spec pl11x] config {
	<start name="pl11x_drv">
		<resource name="RAM" quantum="2M"/>
		<provides><service name="Framebuffer"/></provides>
	</start>}

append_if [have_spec ps2] config {
	<start name="ps2_drv">
		<resource name="RAM" quantum="1M"/>
		<provides><service name="Input"/></provides>
	</start> }

append config {
	<start name="terminal">
		<resource name="RAM" quantum="2M"/>
		<provides><service name="Terminal"/></provides>
		<config>
			<keyboard layout="de"/>
		</config>
	</start>
	<start name="ram_fs">
		<resource name="RAM" quantum="100M"/>
		<provides><service name="File_system"/></provides>
		<config>
			<content>
				<dir name="home">
					<dir name="user">
						<inline name=".bash_profile">
							cp /bin/make /usr/bin/
							cp /bin/echo /usr/bin/
							cp /bin/mkdir /usr/bin/
							echo "creating build directory..."
}
append config "						/genode/tool/create_builddir $platform BUILD_DIR=/home/build"
append config {
							cd /home/build
}
append config "						echo \"CROSS_DEV_PREFIX=$tool_prefix\" > etc/tools.conf"
append config "                                         $platform_cmds"
append config {
							echo "now you can type, for example, \"make core\""
						</inline>
					</dir>
				</dir>
				<dir name="tmp" />
				<dir name="usr">
					<dir name="bin" />
				</dir>
			</content>

			<!-- constrain sessions according to their labels -->
			<policy label="noux -> root" root="/" />
			<policy label="noux -> home" root="/home/user" writeable="yes" />
			<policy label="noux -> tmp"  root="/tmp"       writeable="yes" />
			<policy label="noux -> usr"  root="/usr"       writeable="yes" />
		</config>
	</start>
	<start name="noux">
		<resource name="RAM" quantum="1G" />
		<config>
			<fstab> }

foreach pkg $noux_pkgs {
	append config "		<tar name=\"$pkg.tar\" />" }

append config {			<dir name="platform"> }
foreach pkg $platform_pkgs {
	append config "			<dir name=\"$pkg\">
						<tar name=\"$pkg.tar\" />
					</dir>" }

append config {			</dir>

				<dir name="genode">
					<tar name="genode.tar" />
				</dir>

				<dir name="dev">
					<null />
				</dir>

				<dir name="home">
					<fs label="home" />
				</dir>

				<dir name="tmp">
					<fs label="tmp" />
				</dir>

				<dir name="usr">
					<fs label="usr" />
				</dir>
			</fstab>
			<start name="/bin/bash">
				<env name="TERM" value="linux" />
				<env name="HOME" value="/home" />
				<env name="PATH" value="/bin" />
				<arg value="--login" />
			</start>
		</config>
	</start>
</config>
}

install_config $config


#
# Boot modules
#

# generic modules
set boot_modules {
	core init timer ld.lib.so noux terminal ram_fs
	libc.lib.so libm.lib.so libc_noux.lib.so ncurses.lib.so }

append boot_modules { gmp.lib.so mpfr.lib.so mpc.lib.so }

foreach pkg $noux_pkgs {
	lappend boot_modules "$pkg.tar" }

append boot_modules { genode.tar }

foreach pkg $platform_pkgs { append boot_modules "$pkg.tar" }

# platform-specific modules
lappend_if [have_spec linux] boot_modules fb_sdl
lappend_if [have_spec pci]   boot_modules pci_drv
lappend_if [have_spec vesa]  boot_modules vesa_drv
lappend_if [have_spec ps2]   boot_modules ps2_drv
lappend_if [have_spec pl11x] boot_modules pl11x_drv

build_boot_image $boot_modules

append qemu_args " -m 768 "

run_genode_until forever
