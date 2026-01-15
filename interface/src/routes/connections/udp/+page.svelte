<script lang="ts">
	import type { PageData } from '../$types';
	import UdpSender from './UdpSender.svelte';

	interface Props {
		data: PageData;
	}

	let { data }: Props = $props();
</script>

<UdpSender />
