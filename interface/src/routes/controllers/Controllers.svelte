<script lang="ts">
	import ControllerRestComp from '$lib/components/ControllerRestComp.svelte';
</script>

<ControllerRestComp
	label="Azimuth"
	restPath="/rest/heliostat/azimuth"
	>
</ControllerRestComp>

<ControllerRestComp 
	label="Elevation"
	restPath="/rest/heliostat/elevation"
	>
</ControllerRestComp>