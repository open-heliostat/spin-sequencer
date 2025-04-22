<script lang="ts">
	import CanConsoleRestComp from '$lib/components/CANConsoleRestComp.svelte';
	import CanSettingsRestComp from '$lib/components/CANSettingsRestComp.svelte';
	import type { PageData } from '../$types';

	export let data: PageData;
</script>

<div
	class="mx-0 my-1 flex flex-col space-y-4
     sm:mx-8 sm:my-8"
>
	<CanConsoleRestComp restPath={'/rest/can'} />
	<CanSettingsRestComp restPath={'/rest/can'} />
</div>
