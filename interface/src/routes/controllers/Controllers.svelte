<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Hand from '~icons/tabler/hand-move';
	import Settings from '~icons/tabler/adjustments';
	import { socket } from '$lib/stores/socket';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import Text from '$lib/components/Text.svelte';
	import Collapsible from '$lib/components/Collapsible.svelte';
	import ControllerSettings from '$lib/components/ControllerSettings.svelte';
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