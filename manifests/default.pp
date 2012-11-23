Exec { path => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ] }

class system-update{
	exec { 'apt-get update':
		command => "/usr/bin/apt-get -y update",
		before => Exec["apt-get upgrade"],
	}
  
	exec { 'apt-get upgrade':
		command => "/usr/bin/apt-get -y upgrade && /usr/bin/touch /root/upgraded",
		unless => "/usr/bin/test -s /root/updated"
    }
}

class opengl {
	Package { ensure => "installed" }
	
	$dependencies = ["freeglut3-dev", "freeglut3", "binutils-gold", "glew-utils", "libglew1.6-dev", "libfreeimage-dev", "build-essential", "g++"]
	
	package { $dependencies:
		require => Exec['apt-get update'],
	}		
}

package {'mercurial':
	ensure => installed,
}

include system-update
include opengl