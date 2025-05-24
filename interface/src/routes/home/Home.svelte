<script lang="ts">
	import Restore from '$lib/components/RestoreComp.svelte';
import StatusComp from '$lib/components/StatusComp.svelte';
</script>

<StatusComp restPath="rest/spin-seq/diag" />
<Restore></Restore>